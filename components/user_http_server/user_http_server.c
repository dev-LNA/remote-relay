/*
 * User defined http server library
 * Authors: Rafael M. Silva (rsilva@lna.br)
 *         
 */
#include "user_http_server.h"

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