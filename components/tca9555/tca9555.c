/*
 * User defined library for coonttrol of TCA9555
 * Authors: Rafael M. Silva (rsilva@lna.br)
 *         
 */

#include <stdint.h>
#include <stdbool.h>

#include "driver/gpio.h"
#include "driver/i2c_master.h"
#include "esp_err.h"

#include "user_i2c.h"
#include "tca9555.h"


// -------------------------------------------------------------------
// TCA9555 Configuration 
// - device: Device handler for communitation with
// - bits: 16 bits register where each bit control TCA IO operation mode
// -- 1: for input
// -- 0: for output
void tca_config_mode(i2c_master_dev_handle_t device, uint16_t bits)
{
    // Send most significant bits
    uint16_t data = ((0xFF00 & bits) >> 8);
    ESP_ERROR_CHECK(i2c_send_byte(device,TCA9555_CONFIG_PORT1,(uint8_t)data));

    // Send last significant bits
    data = ((0x00FF & bits));
    ESP_ERROR_CHECK(i2c_send_byte(device,TCA9555_CONFIG_PORT0,(uint8_t)data));

    return;
}

// -------------------------------------------------------------------
// TCA9555 Set (High level) outputs 
// - device: Device handler for communitation with
// - bits: 16 bits register where each bit control the output level
// -- 1: Set for high level
// -- 0: Keep the output in previous state
void tca_set_outputs(i2c_master_dev_handle_t device, uint16_t bits)
{
    uint8_t read_ports[2] = {0};
    // Read the port actual status
    // TCA9555_IN_PORTx reflects the incoming logic levels of the pins,
    // regardless of whether the pin is IN ou OUT
    i2c_receive(device, TCA9555_IN_PORT0, read_ports, 2); 

    // Send most significant bits
    uint8_t data = (uint8_t)((0xFF00 & bits) >> 8);
    data = data | read_ports[1]; // set the outputs from PORT1
    ESP_ERROR_CHECK(i2c_send_byte(device,TCA9555_OUT_PORT1,(uint8_t)data));

    // Send last significant bits
    data = (uint8_t)(0x00FF & bits);
    data = data | read_ports[0]; // set the outputs from PORT0
    ESP_ERROR_CHECK(i2c_send_byte(device,TCA9555_OUT_PORT0,(uint8_t)data));

    return;
}

// -------------------------------------------------------------------
// TCA9555 Clear (low level) outputs 
// - device: Device handler for communitation with
// - bits: 16 bits register where each bit control the output level
// -- 1: Set for low level
// -- 0: Keep the output in previous state
void tca_clear_outputs(i2c_master_dev_handle_t device, uint16_t bits)
{
    uint8_t read_ports[2] = {0};
    // Read the port actual status
    // TCA9555_IN_PORTx reflects the incoming logic levels of the pins,
    // regardless of whether the pin is IN ou OUT
    i2c_receive(device, TCA9555_IN_PORT0, read_ports, 2);

    // Send most significant bits (PORT1)
    uint8_t data = (uint8_t)((0xFF00 & bits) >> 8);
    data ^= 0xFF; // invert the data 
    data = data & read_ports[1]; // clear the outputs from PORT1
    ESP_ERROR_CHECK(i2c_send_byte(device,TCA9555_OUT_PORT1,(uint8_t)data));

    data = (uint8_t)(0x00FF & bits);
    data ^= 0xFF; // invert the data
    data = data & read_ports[0]; // clear the outputs from PORT0    
    ESP_ERROR_CHECK(i2c_send_byte(device,TCA9555_OUT_PORT0,(uint8_t)data));

    return;
}

// -------------------------------------------------------------------
// TCA9555 Clear (low level) outputs 
// - device: Device handler for communitation with
// - bits: 16 bits register where each bit control the output level
// -- 1: Set for low level
// -- 0: Keep the output in previous state
uint16_t tca_get_outputs(i2c_master_dev_handle_t device)
{
    uint8_t read_ports[2] = {0};
    uint16_t bits = 0;
    // Read the port actual status
    // TCA9555_IN_PORTx reflects the incoming logic levels of the pins,
    // regardless of whether the pin is IN ou OUT
    i2c_receive(device, TCA9555_IN_PORT0, read_ports, 2);

    bits = (0xFF & read_ports[1]) << 8; // Get MSBs
    bits = bits | (0xFF & read_ports[0]); // Get LSBs

    return bits;
}


// -------------------------------------------------------------------
// EOF
// -------------------------------------------------------------------