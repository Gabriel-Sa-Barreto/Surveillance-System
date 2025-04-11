#include "sntp.h"

const char *sntp_server = "pool.ntp.org";
#define NUM_OF_SERVERS 1

void time_sync_notification_cb(struct timeval *tv)
{
    ESP_LOGI(SNTP_TAG, "Notification of a time synchronization event");
}

void SNTP_init(void)
{
    ESP_LOGI(SNTP_TAG, "Initializing SNTP");
    esp_sntp_config_t config;
    config.smooth_sync                = false;                                      
    config.wait_for_sync              = true;                                                                 
    config.num_of_servers             = NUM_OF_SERVERS;        
    config.servers[0]                 = sntp_server;
    config.ip_event_to_renew          = IP_EVENT_STA_GOT_IP;   
    #if LWIP_DHCP_GET_NTP_SRV
        config.start                      = false;  // start SNTP service explicitly (after connecting)
        config.server_from_dhcp           = true;   // accept NTP offers from DHCP server, if any (need to enable *before* connecting)
        config.renew_servers_after_new_IP = true;   // let esp-netif update configured SNTP server(s) after receiving DHCP lease
        config.index_of_first_server      = 1;      // updates from server num 1, leaving server 0 (from DHCP) intact
    #else
        config.start                      = true;
        config.server_from_dhcp           = false;
        config.renew_servers_after_new_IP = false;
        config.index_of_first_server      = 0;   
    #endif
    config.sync_cb = time_sync_notification_cb;   
    esp_netif_sntp_init(&config);
}

/* Only if the NTP Address is required by DHCP */
void SNTP_start(void)
{
    #if LWIP_DHCP_GET_NTP_SRV
        ESP_LOGI(SNTP_TAG, "Starting SNTP");
        esp_netif_sntp_start();
    #else
        ESP_LOGI(SNTP_TAG, "Initializing and starting SNTP");
    #endif
    // Wait until the time is set
    time_t    now         = 0;
    struct tm timeinfo    = { 0 };
    int       retry       = 0;
    const int retry_count = 15;
    
    while (esp_netif_sntp_sync_wait(pdMS_TO_TICKS(2000)) == ESP_ERR_TIMEOUT && ++retry < retry_count) { 
        ESP_LOGI(SNTP_TAG, "Waiting for system time to be set... (%d/%d)", retry, retry_count);
    }

    if(sntp_get_sync_status() == SNTP_SYNC_STATUS_COMPLETED)
    {
        char strftime_buf[64];
        setenv("TZ", "GMT+3", 1);
        tzset();
        time(&now);
        localtime_r(&now, &timeinfo);
        strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
        ESP_LOGI(SNTP_TAG, "The current date/time is: %s", strftime_buf);
    }else
    {
        ESP_LOGE(SNTP_TAG, "Error on synchronization time process");
        esp_restart();
    }
    esp_netif_sntp_deinit();
}

void getTime(char *ptr_time, size_t size)
{
    time_t    now      = 0;
    struct tm timeinfo = { 0 };
    time(&now);
    localtime_r(&now, &timeinfo);
    strftime(ptr_time, size, "%I:%M%p", &timeinfo);
}

void getDate(char *ptr_date, size_t size)
{
    time_t    now      = 0;
    struct tm timeinfo = { 0 };

    time(&now);
    localtime_r(&now, &timeinfo);
    strftime(ptr_date, size, "%d/%m", &timeinfo);
}

