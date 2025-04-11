#ifndef __CONNECT_H
    #define __CONNECT_H
    #include "../../main/headers/general.h"
    #include <freertos/FreeRTOS.h>
    #include <freertos/event_groups.h>
    #include "freertos/semphr.h"
    #include "esp_err.h"
    #include <stdbool.h>
    #include "esp_wifi.h"
    #include "esp_event.h"
    #include "esp_log.h"
    #include "lwip/err.h"
    #include "lwip/sys.h"
    #include "../../components/sntp/sntp.h"

    esp_err_t wifi_disconnect(void);
    bool      wifi_isConnected(void);
    void      wifi_connect(void);
    esp_err_t initWifi(void);

#endif // __CONNECT_H