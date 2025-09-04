
/*
 * Remote relay:
 * Project for load remote control using 
 * WT32-ETH0 and MQTT
 * Autors: Rafael M. Silva (rsilva@lna.br)
 *         
 */

// ------------------------------------------------
// Includes 
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "esp_log.h"
#include "esp_err.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"

#include "nvs_flash.h"

#include "user_ethernet.h"
#include "user_mqtt.h"
#include "driver/i2c_master.h"
#include "user_i2c.h"
#include "tca9555.h"

// ------------------------------------------------
// Defines

// ------------------------------------------------
// Global variables
static const char* TAG = "MAIN";
TaskHandle_t relayTaskHandle = NULL;
extern QueueHandle_t v_relay_value_queue;

// ------------------------------------------------
// Local functions
void vRelayHandler(void* pvParameters);


// ------------------------------------------------
// Main function
void app_main(void)
{
    esp_err_t err = nvs_flash_init();
    if(err == ESP_ERR_NVS_NO_FREE_PAGES || 
       err == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init(); 
    } 
    ESP_ERROR_CHECK(err);

    // ------------------------------------------------
    // I2C initialization 
    ESP_LOGI(TAG,"I2C initialization ...");
    i2c_master_bus_handle_t i2c0BusHandler = NULL;
    ESP_ERROR_CHECK(i2c_bus_init(I2C0_PORT,I2C0_SCL_IO,I2C0_SDA_IO,&i2c0BusHandler));
    i2c_master_dev_handle_t i2c0TCA9555 = NULL;
    ESP_ERROR_CHECK(i2c_attach_device(TCA9555_ADDR,i2c0BusHandler,&i2c0TCA9555));

    if(i2c_master_probe(i2c0BusHandler,TCA9555_ADDR,10) == ESP_OK)
    {
        ESP_LOGI(TAG,"Device at address 0x%x  was detected.",TCA9555_ADDR);
        tca_config_mode(i2c0TCA9555, 0x0000); // Define all pins as outputs
        tca_clear_outputs(i2c0TCA9555,0xFFFF); // Clear all pins
    }
    else 
        ESP_LOGW(TAG,"!!!! Failure to initialize I2C bus device !!!!");
    ESP_LOGI(TAG,"Done.");

    // -------------------------------------------
    // Initialize WiFi Station
    ESP_LOGI(TAG,"Ethernet initialization.");
    ESP_ERROR_CHECK(ethernet_setup());
    // -------------------------------------------
    // Initialize MQTT
    ESP_LOGI(TAG,"MQTT initialization.");
    if(user_mqtt_start() == ESP_OK)
    {
        // Write on relay/topic status and change relay to "online"
        user_mqtt_publish(RELAY_TOPIC_STATUS,"Online",1,true);

        // Subscribe on topi "relay/value" to control digital outputs values
        user_mqtt_subscribe(RELAY_TOPIC_VALUE,1);   

        // Create an task to handle MQTT relay data
        xTaskCreate(vRelayHandler,"Relay",configMINIMAL_STACK_SIZE+1024,
                    (void*)i2c0TCA9555,1,&relayTaskHandle);
    }
    else 
        ESP_LOGE(TAG,"!!!! Failure to start MQTT !!!!");

    
    while(true)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

}


// ------------------------------------------------
// Task for handle MQTT topic "relay/value"
void vRelayHandler(void* pvParameters)
{
    // Get the I2C device hadler to work with 
    i2c_master_dev_handle_t tca =  (i2c_master_dev_handle_t)pvParameters;
    while(true)
    {
        int32_t rawData = 0;
        uint16_t relayData = 0;
        xQueueReceive(v_relay_value_queue,&rawData,portMAX_DELAY);

        relayData = (uint16_t)(0x0000FFFF&rawData);
        printf("Relay values: %d\n",relayData);

        // Change TCA Status
        tca_set_outputs(tca,relayData);   //  Set gpio outputs
        tca_clear_outputs(tca,~relayData); // Clear complementar gpio outputs
    }
}

// ------------------------------------------------
// EOF
// ------------------------------------------------