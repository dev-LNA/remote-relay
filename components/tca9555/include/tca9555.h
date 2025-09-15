#ifndef TCA9555_H
#define TCA9555_H

#define TCA9555_ADDR 0x20

// ----------------------------------------------------------------------------------
// Register map
#define TCA9555_IN_PORT0    0x00 // Input Port 0 Read byte
#define TCA9555_IN_PORT1    0x01 // Input Port 1 Read byte
#define TCA9555_OUT_PORT0   0x02 // Output Port 0 Read-write byte
#define TCA9555_OUT_PORT1   0x03 // Output Port 1 Read-write byte
#define TCA9555_POL_INV_PORT0 0x04 // Polarity Inversion Port 0 Read-write byte
#define TCA9555_POL_INV_PORT1 0x05 // Polarity Inversion Port 1 Read-write byte

// If a bit is set to 1, the corresponding port pin is an input 
// If a bit is cleared to 0, the corresponding port pin is an output.
#define TCA9555_CONFIG_PORT0 0x06 // Configuration Port 0 Read-write byte
#define TCA9555_CONFIG_PORT1 0x07 // Configuration Port 1 Read-write byte


#define TCA_P0B0 0x0000 // 00000000 00000001
#define TCA_P0B1 0x0002 // 00000000 00000010
#define TCA_P0B2 0x0004 // 00000000 00000100
#define TCA_P0B3 0x0008 // 00000000 00001000
#define TCA_P0B4 0x0010 // 00000000 00010000
#define TCA_P0B5 0x0020 // 00000000 00100000
#define TCA_P0B6 0x0040 // 00000000 01000000
#define TCA_P0B7 0x0080 // 00000000 10000000

#define TCA_P1B0 0x0100 // 00000001 00000000
#define TCA_P1B1 0x0200 // 00000010 00000000
#define TCA_P1B2 0x0400 // 00000100 00000000
#define TCA_P1B3 0x0800 // 00001000 00000000
#define TCA_P1B4 0x1000 // 00010000 00000000
#define TCA_P1B5 0x2000 // 00100000 00000000
#define TCA_P1B6 0x4000 // 01000000 00000000
#define TCA_P1B7 0x8000 // 10000000 00000000

void tca_config_mode(i2c_master_dev_handle_t device, uint16_t bits);
void tca_set_outputs(i2c_master_dev_handle_t device, uint16_t bits);
void tca_clear_outputs(i2c_master_dev_handle_t device, uint16_t bits);

uint16_t tca_get_outputs(i2c_master_dev_handle_t device);

void tca_set(i2c_master_dev_handle_t device, uint16_t bits);
uint16_t tca_get(i2c_master_dev_handle_t device);

#endif
// ------------------------------------------------
// EOF
// ------------------------------------------------