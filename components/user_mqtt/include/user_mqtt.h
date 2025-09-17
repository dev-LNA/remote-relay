

#ifndef USER_MQTT_H
#define USER_MQTT_H


#define ESP_BROKER_URL "mqtt://192.168.2.101"
#define ESP_BROKER_PORT 1883

#define TOPIC_RELAY_GET     "relay/output/get"    // Nodes subscribe on this to get the relay output values 
#define TOPIC_RELAY_SET     "relay/output/set"    // Nodes publish on this to change the relay outputs
#define TOPIC_RELAY_STATUS  "relay/status"        // Nodes subscribe on this to know if it is "onlyne" or "offline"

esp_err_t user_mqtt_start(void);
void user_mqtt_subscribe(char* topic, int qos);
void user_mqtt_unsubscribe(char* topic);

void user_mqtt_publish(char* topic, char* payload, int qos, bool retain);
bool user_mqtt_con_status(void);
void user_mqtt_stop(void);


#endif
// ------------------------------------------------
// EOF
// ------------------------------------------------