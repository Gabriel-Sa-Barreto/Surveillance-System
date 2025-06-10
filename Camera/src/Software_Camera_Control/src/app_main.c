#include "../../libs/log.h"
#include "../../libs/MQTT.h"
#include "MQTTClient.h"
#include <cjson/cJSON.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>

int main()
{
    /* Log configuration ================================= */
    const char *log_filePath = "/home/pi/Surveillance-System/Camera/src/Software_Camera_Control/log/log_Cam_Control.txt";
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
    else
        log_info("Log system started successfully");
    /* End Log configuration ================================= */
    return 0;
}