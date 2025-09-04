

#include "tca9555.h"

//
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

void tca_set_outputs(i2c_master_dev_handle_t device, uint16_t bits)
{
    uint8_t read_ports[2] = {0};
    // Read the port actual status
    i2c_receive(device, TCA9555_OUT_PORT0, read_ports, 2);

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


void tca_clear_outputs(i2c_master_dev_handle_t device, uint16_t bits)
{
    uint8_t read_ports[2] = {0};
    // Read the port actual status
    i2c_receive(device, TCA9555_OUT_PORT0, read_ports, 2);

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