

#ifndef USER_I2C_H
#define USER_I2C_H

// ----------------------------------------------------------------------------------
// Configuration for I2C BUs 0 
#define I2C0_SCL_IO GPIO_NUM_17     // GPIO number used for I2C master clock
#define I2C0_SDA_IO GPIO_NUM_5     // GPIO number used for I2C master data 
#define I2C0_PORT I2C_NUM_0         // I2C master i2c port number
#define I2C_FREQ_HZ 400000          // I2C master clock frequency
#define I2C0_TX_BUFFER_DISABLE 0    // I2C master doesn't need buffer 
#define I2C0_RX_BUFFER_DISABLE 0    // I2C master doesn't need buffer  

// ----------------------------------------------------------------------------------
// General configuratinos 
#define ACK_CHECK_EN                0x1   // I2C master will check ack from slave
#define ACK_CHECK_DIS               0x0   // I2C master will not check ack from slave
#define ACK_VAL                     0x0   // I2C ack value
#define NACK_VAL                    0x1   // I2C nack value
#define I2C_FREQ_HZ 400000            // I2C master clock frequency

// ----------------------------------------------------------------------------------
// Functions prototypes
esp_err_t i2c_bus_init(i2c_port_num_t i2c_port,gpio_num_t scl, gpio_num_t sda,i2c_master_bus_handle_t* i2cBusHandle);
esp_err_t i2c_attach_device(uint16_t device_address, i2c_master_bus_handle_t i2cBusHandle, i2c_master_dev_handle_t* i2cDevHandle);

esp_err_t i2c_send_byte(i2c_master_dev_handle_t device, uint8_t address, uint8_t data);
esp_err_t i2c_receive_byte(i2c_master_dev_handle_t device, uint8_t address, uint8_t* data);
esp_err_t i2c_receive(i2c_master_dev_handle_t device, uint8_t address, uint8_t* data, size_t size);


#endif


