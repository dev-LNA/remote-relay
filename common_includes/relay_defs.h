

#ifndef RELAY_DEFS_H
#define RELAY_DEFS_H

#include <stdint.h>
#include <stdbool.h>

typedef enum 
{
    TCA_READ,  // Read status operation 
    TCA_WRITE  // Write on output
} i2c_req_type_t;

typedef struct 
{
    i2c_req_type_t type;
    int32_t       data;
} tca_data_exchange_t;

#endif