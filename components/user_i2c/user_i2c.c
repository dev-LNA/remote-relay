/*
 * User defined I2C library
 * Authors: Rafael M. Silva (rsilva@lna.br)
 *         
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "driver/gpio.h"
#include "driver/i2c_master.h"
#include "user_i2c.h"

// ----------------------------------------------------------------------------------
// I2C bus initialization
esp_err_t i2c_bus_init(i2c_port_num_t i2c_port,gpio_num_t scl, gpio_num_t sda,
    i2c_master_bus_handle_t* i2cBusHandle)
{
    esp_err_t err = ESP_OK;
    i2c_master_bus_config_t i2cMasterCfg = 
    {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port   = i2c_port,
        .scl_io_num = scl,
        .sda_io_num = sda,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true, 
    };
    err = i2c_new_master_bus(&i2cMasterCfg,i2cBusHandle);

    return err;
}

// ----------------------------------------------------------------------------------
// I2C Bus attach a device
esp_err_t i2c_attach_device(uint16_t device_address, i2c_master_bus_handle_t i2cBusHandle, 
    i2c_master_dev_handle_t* i2cDevHandle)
{

    esp_err_t err = ESP_OK;
    i2c_device_config_t i2cDevCfg = 
    {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address  = device_address,
        .scl_speed_hz    = I2C_FREQ_HZ
    };
    
    err = i2c_master_bus_add_device(i2cBusHandle,&i2cDevCfg,i2cDevHandle);

    return err;
}


// ----------------------------------------------------------------------------------
// I2C send a byte
esp_err_t i2c_send_byte(i2c_master_dev_handle_t device, uint8_t address, uint8_t data)
{
    esp_err_t err = ESP_OK;
    uint8_t reg[] = {0, 0};

    reg[0] = address;
    reg[1] = data;
    err = i2c_master_transmit(device,reg,2,50);

    return err;
}

// ----------------------------------------------------------------------------------
// I2C receive a byte
esp_err_t i2c_receive_byte(i2c_master_dev_handle_t device, uint8_t address, uint8_t* data)
{
    esp_err_t err = ESP_OK;

    err = i2c_master_transmit_receive(device,&address,1,data,1,50);

    return err;
}

// ----------------------------------------------------------------------------------
// I2C receive multiple bytes
esp_err_t i2c_receive(i2c_master_dev_handle_t device, uint8_t address, uint8_t* data, size_t size)
{
    esp_err_t err = ESP_OK;

    err = i2c_master_transmit_receive(device,&address,1,data,size,50);

    return err;

}
// ------------------------------------------------
// EOF
// ------------------------------------------------