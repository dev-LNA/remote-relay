/*
 * User defined http server library
 * Authors: Rafael M. Silva (rsilva@lna.br)
 *         
 */
#include "user_http_server.h"

static const char* TAG = "HTTP SERVER";
static bool outputs[16] = {0};

extern QueueHandle_t v_relay_get_queue;
extern QueueHandle_t v_relay_set_queue;

static const char index_html[] =
"<!DOCTYPE html>"
"<html>"
"<head>"
"  <meta charset='utf-8'>"
"  <title>Output status</title>"
"  <style>"
"    body {"
"      font-family: Arial, sans-serif;"
"      text-align: center;"
"      margin: 0;"
"      padding: 20px;"
"      background: #f4f6f8;"
"      color: #333;"
"    }"
"    h2 {"
"      margin-bottom: 20px;"
"    }"
"    .grid {"
"      display: grid;"
"      grid-template-columns: repeat(8, 70px);"
"      grid-gap: 15px;"
"      justify-content: center;"
"      margin-bottom: 20px;"
"    }"
"    .btn {"
"      width: 60px; height: 60px;"
"      border-radius: 50%;"
"      border: none;"
"      cursor: pointer;"
"      color: #fff;"
"      font-size: 14px;"
"      font-weight: bold;"
"      box-shadow: 0 3px 6px rgba(0,0,0,0.2);"
"      transition: transform 0.15s, box-shadow 0.2s;"
"    }"
"    .btn:active {"
"      transform: scale(0.92);"
"      box-shadow: 0 2px 4px rgba(0,0,0,0.2);"
"    }"
"    .on {"
"      background: linear-gradient(145deg, #28a745, #218838);"
"    }"
"    .off {"
"      background: linear-gradient(145deg, #dc3545, #c82333);"
"    }"
"  </style>"
"</head>"
"<body>"
"  <h2>Remote Relay WEB Control</h2>"
"  <div class='grid' id='row1'></div>"
"  <div class='grid' id='row2'></div>"
"  <script>"
"    function createButtons() {"
"      for (let i = 0; i < 16; i++) {"
"        let btn = document.createElement('button');"
"        btn.id = 'btn'+i;"
"        btn.className = 'btn off';"
"        btn.innerText = i;"
"        btn.onclick = () => toggle(i);"
"        if (i < 8) document.getElementById('row1').appendChild(btn);"
"        else document.getElementById('row2').appendChild(btn);"
"      }"
"    }"
"    function toggle(pin) {"
"      fetch(`/toggle?pin=${pin}`)"
"      .then(r => r.json())"
"      .then(data => updateUI(data));"
"    }"
"    function updateUI(states) {"
"      for (let pin in states) {"
"        let btn = document.getElementById('btn'+pin);"
"        if (states[pin] == 1) {"
"          btn.classList.remove('off');"
"          btn.classList.add('on');"
"        } else {"
"          btn.classList.remove('on');"
"          btn.classList.add('off');"
"        }"
"      }"
"    }"
"    function refresh() {"
"      fetch('/status')"
"      .then(r => r.json())"
"      .then(data => updateUI(data));"
"    }"
"    createButtons();"
"    setInterval(refresh, 2000);"
"    refresh();"
"  </script>"
"</body>"
"</html>";

// ------------------------------------------------
// Handler of initial (index) page
static esp_err_t index_handler(httpd_req_t *req)
{
    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, index_html, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

// ------------------------------------------------
// Handler of /status → returning a JSON
// Status page handler, the webpage buttons status are 
// updated according to the output states 
static esp_err_t status_handler(httpd_req_t *req)
{
    char buffer[256];
    int offset = 0;
    uint16_t relayData = 0;
    tca_data_exchange_t x_relay_http;

    x_relay_http.type = TCA_READ;
    xQueueSend(v_relay_set_queue,&x_relay_http,pdMS_TO_TICKS(50)); // request TCA data
    
    xQueueReceive(v_relay_get_queue,&x_relay_http,pdMS_TO_TICKS(300)); // receive TCA data
    
    ESP_LOGI(TAG,"Received data: %d",x_relay_http.data);
    relayData = (uint16_t)(x_relay_http.data&0x0000FFFF);

    ESP_LOGI(TAG,"Status: %d\n", relayData);    
    offset += snprintf(buffer + offset, sizeof(buffer) - offset, "{"); // create a JSON File to update the web page
    for (int i = 0; i < 16; i++) 
    {
        outputs[i] = (bool)((relayData >> i) & 0x0001); // update outputs according to TCA state

        offset += snprintf(buffer + offset, sizeof(buffer) - offset,
            "\"%d\":%d%s", i, outputs[i], (i < 15 ? "," : ""));
    }
    offset += snprintf(buffer + offset, sizeof(buffer) - offset, "}");
    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, buffer, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}


// ------------------------------------------------
// Handler of /toggle?pin=X
// change the button status on WEB page click
static esp_err_t toggle_handler(httpd_req_t *req)
{
    char query[32];
    uint16_t relayData = 0;
    tca_data_exchange_t x_relay_http;

    if (httpd_req_get_url_query_str(req, query, sizeof(query)) == ESP_OK) 
    {
        char param[8];
        if (httpd_query_key_value(query, "pin", param, sizeof(param)) == ESP_OK) 
        {
            int pin = atoi(param);
            if (pin >= 0 && pin < 16) 
            {
                outputs[pin] = !outputs[pin]; // 
                ESP_LOGI(TAG, "Toggle pin %d -> %d", pin, outputs[pin]);
                // Queue to write the value on TCA9555 
                for(int i = 0; i< 16; i++)
                    relayData = relayData | (outputs[i] << i);
                printf("HTTP Toggle: 0x%x\n",relayData);

                x_relay_http.type = TCA_WRITE;
                x_relay_http.data = relayData;
                xQueueSend(v_relay_set_queue,&x_relay_http,portMAX_DELAY); // request TCA data
            }
        }
    }

    char buffer[256];
    int offset = 0;
    offset += snprintf(buffer + offset, sizeof(buffer) - offset, "{");
    for (int i = 0; i < 16; i++) 
    {
        offset += snprintf(buffer + offset, sizeof(buffer) - offset,
                           "\"%d\":%d%s", i, outputs[i], (i < 15 ? "," : ""));
    }
    offset += snprintf(buffer + offset, sizeof(buffer) - offset, "}");
    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, buffer, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}


// ------------------------------------------------
httpd_handle_t start_webserver(void)
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_handle_t server = NULL;

    if (httpd_start(&server, &config) == ESP_OK) 
    {
        httpd_uri_t uri_index = 
        {
            .uri       = "/",
            .method    = HTTP_GET,
            .handler   = index_handler,
            .user_ctx  = NULL
        };
        httpd_register_uri_handler(server, &uri_index);

        httpd_uri_t uri_status = 
        {
            .uri       = "/status",
            .method    = HTTP_GET,
            .handler   = status_handler,
            .user_ctx  = NULL
        };
        httpd_register_uri_handler(server, &uri_status);

        httpd_uri_t uri_toggle = 
        {
            .uri       = "/toggle",
            .method    = HTTP_GET,
            .handler   = toggle_handler,
            .user_ctx  = NULL
        };
        httpd_register_uri_handler(server, &uri_toggle);

        ESP_LOGI(TAG, "Servidor iniciado");
    }
    return server;
}

// ------------------------------------------------
// EOF
// ------------------------------------------------
/*
static const char index_html[] =
"<!DOCTYPE html>"
"<html>"
"<head>"
"  <meta charset='utf-8'>"
"  <title>Remote Relay</title>"
"  <style>"
"    body { font-family: Arial; text-align: center; margin-top: 30px; rgba(94, 93, 93, 1);}"
"    .grid {"
"      display: grid;"
"      grid-template-columns: repeat(8, 60px);"
"      grid-gap: 15px;"
"      justify-content: center;"
"      margin-bottom: 20px;"
"    }"
"    .btn {"
"      width: 50px; height: 50px;"
"      border-radius: 50%;"
"      border: none;"
"      cursor: pointer;"
"      color: white;"
"      font-size: 14px;"
"    }"
"    .on { background: green; }"
"    .off { background: red; }"
"  </style>"
"</head>"
"<body>"
"  <h2> Output status </h2>"
"  <div class='grid' id='row1'></div>"
"  <div class='grid' id='row2'></div>"
"  <script>"
"    function createButtons() {"
"      for (let i = 0; i < 16; i++) {"
"        let btn = document.createElement('button');"
"        btn.id = 'btn'+i;"
"        btn.className = 'btn off';"
"        btn.innerText = i;"
"        btn.onclick = () => toggle(i);"
"        if (i < 8) document.getElementById('row1').appendChild(btn);"
"        else document.getElementById('row2').appendChild(btn);"
"      }"
"    }"
"    function toggle(pin) {"
"      fetch(`/toggle?pin=${pin}`)"
"      .then(r => r.json())"
"      .then(data => updateUI(data));"
"    }"
"    function updateUI(states) {"
"      for (let pin in states) {"
"        let btn = document.getElementById('btn'+pin);"
"        if (states[pin] == 1) {"
"          btn.classList.remove('off');"
"          btn.classList.add('on');"
"        } else {"
"          btn.classList.remove('on');"
"          btn.classList.add('off');"
"        }"
"      }"
"    }"
"    function refresh() {"
"      fetch('/status')"
"      .then(r => r.json())"
"      .then(data => updateUI(data));"
"    }"
"    createButtons();"
"    setInterval(refresh, 2000);"
"    refresh();"
"  </script>"
"</body>"
"</html>"; */