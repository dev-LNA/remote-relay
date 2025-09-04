

#ifndef USER_MQTT_H
#define USER_MQTT_H


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "esp_log.h"
#include "mqtt_client.h"

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"

// #include "json_generator.h"

#define ESP_BROKER_URL "mqtt://192.168.2.103"
#define ESP_BROKER_PORT 1883

#define JSON_BUFFER_SIZE 1024
#define RELAY_TOPIC_STATUS "relay/status"
#define RELAY_TOPIC_VALUE  "relay/value"
#define RELAY_TOPIC_LED1   "relay/led1"
#define RELAY_TOPIC_LED2   "relay/led2"

esp_err_t user_mqtt_start(void);
void user_mqtt_subscribe(char* topic, int qos);
void user_mqtt_unsubscribe(char* topic);

void user_mqtt_publish(char* topic, char* payload, int qos, bool retain);
bool user_mqtt_con_status(void);
void user_mqtt_stop(void);


#endif