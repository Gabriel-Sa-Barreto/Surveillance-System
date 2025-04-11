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

    /* Constants to reporting the MQTT actions */
    #define NETWORK_CONNECTED   BIT1
    #define MQTT_CONNECTED      BIT2
    #define MQTT_PUBLISHED      BIT3
    #define MQTT_PUBLISH_MSG    BIT4
    #define MQTT_DATA_RECEIVED  BIT5
    #define RECONNECT           BIT6

    void           task_MQTT_Controller(void *args);
    bool           mqtt_connected(void);
    void           mqtt_event_handler_callback(esp_mqtt_event_handle_t event_data);
    void           mqtt_connection();
    uint8_t        mqtt_publish(char *topic, char *msg);
    TaskHandle_t*  get_mqttHandle();
    QueueHandle_t* getHandler_MQTTpackageQueue();
#endif