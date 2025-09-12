
/*
 * Remote relay:
 * Project for load remote control using 
 * WT32-ETH0 and MQTT
 * Authors: Rafael M. Silva (rsilva@lna.br)
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

#include "user_http_server.h"

#include "relay_defs.h"

// ------------------------------------------------
// Defines

// ------------------------------------------------
// Global variables
static const char* TAG = "MAIN";
TaskHandle_t relayTaskHandle = NULL;
QueueHandle_t v_relay_get_queue;
QueueHandle_t v_relay_set_queue;

// relay_status_t relay_flags = RELAY_FLAGS_DEFAULT;
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
    // Create a queue for transfer data between tasks with TCA9555
    v_relay_get_queue  = xQueueCreate(5,sizeof(tca_data_exchange_t));
    v_relay_set_queue  = xQueueCreate(5,sizeof(tca_data_exchange_t));

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
        vTaskDelay(pdMS_TO_TICKS(100)); // wait 
        tca_config_mode(i2c0TCA9555, 0x0000);   // Define all pins as outputs
        tca_clear_outputs(i2c0TCA9555,0xFFFF);  // Clear gpio outputs

        // Create a task to handle relay data exchange with I2C
        xTaskCreate(vRelayHandler,"Relay",configMINIMAL_STACK_SIZE+1024,
                    (void*)i2c0TCA9555,5,&relayTaskHandle);
    }
    else 
        ESP_LOGW(TAG,"Failure to initialize I2C bus device !!!!");
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
    }
    else 
        ESP_LOGE(TAG,"Failure to start MQTT !!!!");

    
    // -------------------------------------------
    // Start web server
    start_webserver();

    while(true)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

}


// ------------------------------------------------
// Task for handle TCA9555
void vRelayHandler(void* pvParameters)
{
    // Get the I2C device hadler to work with 
    i2c_master_dev_handle_t tca =  (i2c_master_dev_handle_t)pvParameters;
    tca_data_exchange_t rawData;
    uint16_t relayData = 0;
    static uint16_t actualData = 0;
    while(true)
    {
        xQueueReceive(v_relay_set_queue,&rawData,portMAX_DELAY); // Wait for a requisition (read or write)

        if(rawData.type == TCA_WRITE) 
        {
            relayData = (uint16_t)(0x0000FFFF&(rawData.data));
            tca_set_outputs(tca,relayData);      //  Set gpio outputs
            tca_clear_outputs(tca,~relayData);   // Clear complementar gpio outputs
            actualData = tca_get_outputs(tca);   // Read the actual status from I2C 
        }
        else if(rawData.type == TCA_READ)
        {
            rawData.data = actualData;
            xQueueSend(v_relay_get_queue,&rawData,100);
        }
        printf("Relay values: 0x%x\n",actualData);
    }
}



// ------------------------------------------------
// EOF
// ------------------------------------------------