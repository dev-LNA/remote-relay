

#ifndef USER_MQTT_H
#define USER_MQTT_H


#define ESP_BROKER_URL "mqtt://192.168.2.101"
#define ESP_BROKER_PORT 1883


#define TOPIC_RELAY_GET     "relay/get"
#define TOPIC_RELAY_SET     "relay/set"
#define TOPIC_RELAY_STATUS  "relay/status"
#define RELAY_TOPIC_STATUS  "relay/status"
#define RELAY_TOPIC_VALUE   "relay/value"


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