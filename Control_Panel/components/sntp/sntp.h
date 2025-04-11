#ifndef __SNTP_H
    #define __SNTP_H
    #include "esp_netif_sntp.h"
    #include "lwip/ip_addr.h"
    #include "esp_sntp.h"
    #include "esp_log.h"
    #include "../../main/headers/general.h"

    void SNTP_init(void);
    void SNTP_start(void);
    void getTime(char *ptr_time, size_t size);
    void getDate(char *ptr_date, size_t size);
#endif