
#ifndef USER_HTTP_SERVER
#define USER_HTTP_SERVER

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "esp_log.h"
#include "esp_http_server.h"

#include "relay_defs.h"

// httpd_handle_t setup_server(void);
httpd_handle_t start_webserver(void);



#endif
// ------------------------------------------------
// EOF
// ------------------------------------------------
