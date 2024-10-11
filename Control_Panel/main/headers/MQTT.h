#ifndef __MQTT_H
    #define __MQTT_H

    #include "stdio.h"
    #include <string.h>
    #include <freertos/FreeRTOS.h>
    #include <freertos/task.h>
    #include "freertos/queue.h"
    #include <freertos/event_groups.h>
    #include "esp_event.h"
    #include "esp_log.h"
    #include "mqtt_client.h"
    #include "cJSON.h"
    #include "general.h"

    #define NETWORK_CONNECTED   BIT1
    #define MQTT_CONNECTED      BIT2
    #define MQTT_PUBLISHED      BIT3
    #define MQTT_PUBLISH_MSG    BIT4
    #define MQTT_ALERT_RECEIVED BIT5
    #define RECONNECT           BIT6

    #define MQTT_TAG "MQTT"

    void task_MQTT_Controller(void *args);
    bool mqtt_connected(void);
    void mqtt_event_handler_callback(esp_mqtt_event_handle_t event_data);
    void mqtt_connection();
    void mqtt_publish(char *topic, char *msg);
    TaskHandle_t* get_mqttHandle();

#endif //__MQTT_H