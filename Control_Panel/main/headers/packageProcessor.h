#ifndef __PACKAGE_PROCESSOR_H
    #define __PACKAGE_PROCESSOR_H
    #include "stdio.h"
    #include "string.h"
    #include <freertos/FreeRTOS.h>
    #include <freertos/task.h>
    #include "freertos/queue.h"
    #include "general.h"
    #include "MQTT.h"
    #include "fileSystem.h"

    void           task_packageProcessor(void *args);
    QueueHandle_t* get_queueHandler_alert();

#endif