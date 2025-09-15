

#ifndef RELAY_DEFS_H
#define RELAY_DEFS_H

#include <stdint.h>
#include <stdbool.h>


// ----------------------------------------------
// Structures for data exchange between tasks and 
// web protrocols
typedef enum 
{
    TCA_READ,  // Read TCA on I2C bus 
    TCA_WRITE  // Write on TCA on I2C bus
} i2c_req_type_t;

typedef struct 
{
    i2c_req_type_t type; // type of requisition
    int32_t        data; // data to be exchanged 
} tca_data_exchange_t;

// ----------------------------------------------

struct CtrlFlagsBits
{
    bool i2c_fail:1;
    bool ethernet_fail:1;
    bool mqtt_fail:1;
    uint16_t RSV:13;
};

typedef union CtrlFlagsReg
{
    uint16_t all;
    struct CtrlFlagsBits bit;
} RelayCtrlFlags;

#define RELAY_FLAGS_DEFAULT 0x0

#endif