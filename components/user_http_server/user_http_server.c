/*
 * User defined http server library
 * Authors: Rafael M. Silva (rsilva@lna.br)
 *         
 */
#include "user_http_server.h"

static const char* TAG = "HTTP SERVER";
static int outputs[] = {0, 0}; 

static const char index_html[] = 
"<!DOCTYPE html>"
"<html>"
"<head>"
"  <meta charset='utf-8'>"
"  <title>Controle Remoto</title>"
"  <style>"
"    body { font-family: Arial; text-align: center; margin-top: 50px; }"
"    .btn {"
"      display: inline-block; padding: 15px 30px; margin: 10px;"
"      font-size: 20px; border-radius: 10px; cursor: pointer; color: white;"
"    }"
"    .on { background: green; }"
"    .off { background: red; }"
"  </style>"
"</head>"
"<body>"
"  <h2>Controle de Cargas</h2>"
"  <div>"
"    <button id='btn0' class='btn off' onclick='toggle(0)'>OUT 0</button>"
"    <button id='btn1' class='btn off' onclick='toggle(1)'>OUT 1</button>"
"  </div>"
"  <script>"
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
"    setInterval(refresh, 2000);"
"    refresh();"
"  </script>"
"</body>"
"</html>";

static esp_err_t index_handler(httpd_req_t *req)
{
    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, index_html, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

static esp_err_t status_handler(httpd_req_t *req)
{
    char buffer[64];
    snprintf(buffer, sizeof(buffer), "{\"0\":%d,\"1\":%d}", outputs[0], outputs[1]);
    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, buffer, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}



static esp_err_t toggle_handler(httpd_req_t *req)
{
    char query[32];
    if (httpd_req_get_url_query_str(req, query, sizeof(query)) == ESP_OK) {
        char param[8];
        if (httpd_query_key_value(query, "pin", param, sizeof(param)) == ESP_OK) {
            int pin = atoi(param);
            if (pin >= 0 && pin < 2) {
                outputs[pin] = !outputs[pin]; // alterna o valor
                ESP_LOGI(TAG, "Toggle pin %d -> %d", pin, outputs[pin]);
                // Aqui no seu código real você faz a escrita no TCA9555
            }
        }
    }
    char buffer[64];
    snprintf(buffer, sizeof(buffer), "{\"0\":%d,\"1\":%d}", outputs[0], outputs[1]);
    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, buffer, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}


httpd_handle_t start_webserver(void)
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_handle_t server = NULL;

    if (httpd_start(&server, &config) == ESP_OK) {
        httpd_uri_t uri_index = {
            .uri       = "/",
            .method    = HTTP_GET,
            .handler   = index_handler,
            .user_ctx  = NULL
        };
        httpd_register_uri_handler(server, &uri_index);

        httpd_uri_t uri_status = {
            .uri       = "/status",
            .method    = HTTP_GET,
            .handler   = status_handler,
            .user_ctx  = NULL
        };
        httpd_register_uri_handler(server, &uri_status);

        httpd_uri_t uri_toggle = {
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

/*
static char resp[] = "<!DOCTYPE html><html> </html> <body>  <h2>ESP32 WEB SERVER</h2> </body>";


static esp_err_t send_web_page(httpd_req_t* req)
{  
    esp_err_t response; 
    response = httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);

    return response;
}

static esp_err_t get_req_handler(httpd_req_t* req)
{
    return send_web_page(req);
}

static httpd_uri_t uri_get = 
{
    .uri = "/",
    .method = HTTP_GET,
    .handler = get_req_handler,
    .user_ctx = NULL
};

httpd_handle_t setup_server(void)
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_handle_t server = NULL;

    if(httpd_start(&server,&config) == ESP_OK)
    {
        httpd_register_uri_handler(server,&uri_get);
    }

    

    return server;
}
   */ 