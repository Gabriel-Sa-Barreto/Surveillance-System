#ifndef __BUTTONS_H
    #define __BUTTONS_H

    #include "general.h"
    #include <freertos/FreeRTOS.h>
    #include <freertos/task.h>
    #include "freertos/queue.h"
    #include "esp_log.h"
    #include "driver/gpio.h"
    
    QueueHandle_t getHandlerQueueBT();
    void task_BTSignalDebounce(void *params);
    void initButtons();

#endif