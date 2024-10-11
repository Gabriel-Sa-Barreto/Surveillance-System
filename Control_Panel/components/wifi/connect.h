#ifndef __CONNECT_H
    #define __CONNECT_H
    #include <stdbool.h>
    #include <string.h>
    #include "freertos/FreeRTOS.h"
    #include "freertos/task.h"
    #include "freertos/event_groups.h"
    #include "esp_wifi.h"
    #include "esp_event.h"
    #include "esp_log.h"
    #include "esp_err.h"
    #include "lwip/err.h"
    #include "lwip/sys.h"
    #include "../../main/headers/general.h"

    #define WIFI_TAG "WIFI"
    
    esp_err_t wifi_disconnect(void);
    bool wifi_isConnected(void);
    void wifi_connect(void);
    esp_err_t initWifi();

#endif // __CONNECT_H