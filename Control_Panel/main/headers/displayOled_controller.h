#ifndef __DISPLAYOLED_CONTROLLER_H
    #define __DISPLAYOLED_CONTROLLER_H

    #include "general.h"
    #include "esp_log.h"
    #include "driver/gpio.h"
    #include <freertos/FreeRTOS.h>
    #include <freertos/task.h>
    #include "freertos/semphr.h"
    #include "packageProcessor.h"
    #include "displayOled.h"
    #include "../components/sntp/sntp.h"
    #include "../components/wifi/connect.h"

    uint16_t DisplayTask_format_screenChange_command(uint8_t buttonPressed, uint8_t screens_ID);
    uint16_t DisplayTask_format_sucess_failureScreen_command(uint8_t screens_ID);
    uint8_t  getCursorPosition(void);
    void     setCursorPosition(uint8_t value);
    void     displayOled_task(void *args);
    void     renderConectionLogo(bool status);
    void     updateTopMenu(void);
    
    enum screens_ID
    {
        SCREEN_INITIAL_SCREEN,
        SCREEN_EN_SYSTEM,
        SCREEN_DIS_SYSTEM,
        SCREEN_CAMERAS_MODE,
        SCREEN_ALERT_PROCESS,
        SCREEN_VALIDATE_PWD,
        SCREEN_OPERATION_SUCCESS,
        SCREEN_OPERATION_FAILURE
    };
    
    enum display_actions
    {
        CHANGE_SCREEN,
        REFRESH_TIME_AND_HOUR
    };

#endif