

#ifndef RELAY_DEFS_H
#define RELAY_DEFS_H

#include <stdint.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

typedef enum 
{
    TCA_READ,  // Read status operation 
    TCA_WRITE  // Write on output
} i2c_req_type_t;

typedef struct 
{
    i2c_req_type_t type;
    uint16_t       bits;
    QueueHandle_t  resp_q; // fila para resposta (criada pela task chamadora)
} tca_exchange_t;

#endif