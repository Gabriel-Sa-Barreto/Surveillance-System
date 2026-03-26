/* Libraries in C++ ========= */
#include <iostream>
#include <opencv2/opencv.hpp>
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
/* NAMESPACES ============== */
using namespace std;
/* MQTT TOPICS ============= */
char *MQTT_TOPIC_SEND_IMAGE = (char*) "cam/img/";
/* PROTOTYPE FUNCTIONS */
int MQTT_on_message(void *context, char *topicName, int topicLen, MQTTClient_message *message);
/* ========================= */
#define SET_DEBUG

int main()
{
    /* Log configuration ================================= */
    const char *log_filePath = "/home/gabriel/Documents/Surveillance-System/Camera/src/Software_getCam_Img/log/log_getCam_Img.txt";
    struct stat buffer;
    bool logFile_exist = stat(log_filePath, &buffer) == 0 ? true : false;
    FILE *fp           = (logFile_exist) ? fopen(log_filePath, "a") : fopen(log_filePath, "w");
    if(fp == NULL)
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

    cv::VideoCapture cap("libcamerasrc ! video/x-raw, width=640, height=480 ! videoconvert ! appsink", cv::CAP_GSTREAMER);
    if(!cap.isOpened())
    {
        #ifdef SET_DEBUG
        log_error("Error to open camera");
        #endif
        return EXIT_FAILURE;
    }
#ifdef SET_DEBUG
    else
        log_info("Camera opened successfully!!");
#endif

    if(mqtt_init(MQTT_on_message) != MQTTCLIENT_SUCCESS)
        return EXIT_FAILURE;

    set_mqtt_msg_debug(false);

    cv::Mat frame;
    vector<uchar> cv_buffer;
    vector<int> params = {cv::IMWRITE_JPEG_QUALITY, 70}; // Aggressive compression for performance.

    while (true) {
        cap >> frame;
        if (frame.empty())
        {
            #ifndef SET_DEBUG
            cout << "Frame is empty" << endl;
            #endif
            break;
        }

        //cv::imshow("Captured Image", frame);
        // Compresses to reduce the MQTT payload.
        cv::imencode(".jpg", frame, cv_buffer, params);
        // Publish the binary buffer.
        mqtt_publish(MQTT_TOPIC_SEND_IMAGE, (char*) cv_buffer.data(), cv_buffer.size(), false);
        // Delay to maintain in 10 FPS.
        cv::waitKey(100); 
    }
    return 0;
}

int MQTT_on_message(void *context, char *topicName, int topicLen, MQTTClient_message *message)
{
    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    return 1;
}