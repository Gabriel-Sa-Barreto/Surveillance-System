#ifndef __PROJECT_TIMERS_H
    #define __PROJECT_TIMERS_H

    #include "freertos/FreeRTOS.h"
    #include "freertos/timers.h"
    #include "esp_log.h"
    #include "../components/wifi/connect.h"
    #include "headers/displayOled_controller.h"
    #include "../components/sntp/sntp.h"

    #define TIMER_RFS_TOP_MENU_inMS 60000        // 60 seconds
    
    void initTimer_rfs_top_menu(void);
#endif