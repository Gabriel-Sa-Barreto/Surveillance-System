#ifndef __GENERAL_H
    #define __GENERAL_H
    #include <freertos/FreeRTOS.h>
    #include <freertos/task.h>
    #include "freertos/queue.h"
    /* Wifi Credentials */
    #define WIFI_SSID ""
    #define WIFI_PWD ""

    /* MQTT Credentials */
    #define MQTT_URI        "mqtt://broker.hivemq.com:1883"
    #define MQTT_CLIENT_ID  "kshdfsfd27/09/2024-15:40"
    #define MQTT_username   ""
    #define MQTT_password   ""  

    /* Communicate topic between the cameras and the control panel */
    #define TOPIC_EN_OR_DIS_SYSTEM     "/system/cameras/activated/"
    #define TOPIC_ALERT                "/system/cameras/alert/"
    #define TOPIC_CAM_MODE             "/system/cameras/mode/"

    /* Input/Output pins */
    #define LED_RED_PIN      5
    #define LED_GREEN_PIN   19
    #define BT_1_PIN        39
    #define BT_2_PIN        34
    #define BT_3_PIN        35

    #define TIMER_ALERT_TAG "ALERT TIMER"
    #define TIMER_STATUS_TAG "STATUS TIMER"

    /* Struct to alerts */
    typedef struct
    {
        int  cameraID;
        char zone[10];
    }alertData;

    typedef struct
    {
        TaskHandle_t   mainTaskHandler;
        QueueHandle_t  alertQueue;
    }mqttParameter;
    
#endif