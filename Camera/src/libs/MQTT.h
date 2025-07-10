#ifndef __DEFINE_MQTT_H
    #define __DEFINE_MQTT_H

    #ifdef __cplusplus
    extern "C" {
    #endif

    #include "MQTTClient.h"
    #include <unistd.h>
    #include <string.h>
    #include "log.h"
    
    #define MQTT_ADDRESS "mqtt://raspberrypi.local:1883" // "mqtt://mqtt.eclipseprojects.io:1883"
    #define MQTT_CLIENT_ID "dfaaerer1546546:16:39-19-05-25"
    #define MQTT_USERNAME "camera"
    #define MQTT_PASSWORD "cameraTest"
    #define MQTT_QOS 2
    #define MQTT_RECONNECTION_TIME 10
    
    int mqtt_init(int (*onMessage)(void *, char *, int, MQTTClient_message *));
    int mqtt_subscribe(char *topic);
    int mqtt_publish(char *topic, char *payload, int retained);

    #ifdef __cplusplus
    }
    #endif

#endif