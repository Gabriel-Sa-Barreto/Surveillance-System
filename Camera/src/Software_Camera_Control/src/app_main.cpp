/* Libraries in C++ ========= */
#include "../src/include/Manage_Camera.h"
#include <thread>
/* ========================== */
/* Libraries in C  ========== */
#include "log.h"
#include "MQTT.h"
#include "MQTTClient.h"
#include <cjson/cJSON.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
/* ========================= */
/* MQTT TOPICS ============= */
char *MQTT_TOPIC_SEND_IMAGE        = (char*) "cam/img/";
char *MQTT_TOPIC_SEND_STREAMING    = (char*) "cam/straming/send/";
char *MQTT_TOPIC_IMG_REQUEST       = (char*) "cam/img/request/";
char *MQTT_TOPIC_STREAMING_REQUEST = (char*) "cam/streaming/";
/* ========================= */

int main()
{
    /* Log configuration ================================= */
    const char *log_filePath = "/home/pi/Documents/Surveillance-System/Camera/src/Software_Camera_Control/log/log_Cam_Control.txt";
    struct stat buffer;
    bool logFile_exist = stat(log_filePath, &buffer) == 0 ? true : false;
    FILE *fp           = (logFile_exist) ? fopen(log_filePath, "a") : fopen(log_filePath, "w");
    if(!fp)
    {
        log_error("Error to open file log.");
        return EXIT_FAILURE;
    }

    if(log_add_fp(fp, LOG_INFO) == -1)
    {
        log_error("Error to add file log pointer.");
        return EXIT_FAILURE;
    }
    else{
        log_info("Log system started successfully");
    }
    /* End Log configuration ================================= */
    /* MQTT Configuration */
    /*if(mqtt_init(MQTT_on_message) != MQTTCLIENT_SUCCESS)
        return EXIT_FAILURE;

    mqtt_subscribe(MQTT_TOPIC_IMG_REQUEST);
    mqtt_subscribe(MQTT_TOPIC_STREAMING_REQUEST);*/
    ManageCamera *mg_cam = new ManageCamera();
    mg_cam->initCamera();
    mg_cam->configureCam();
    mg_cam->startCamera();
    std::this_thread::sleep_for(10000ms);
    mg_cam->stopCameraProcess();
    return 0;
}