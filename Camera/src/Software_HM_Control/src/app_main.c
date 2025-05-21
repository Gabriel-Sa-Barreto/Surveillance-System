#include "../include/controller_led_bt.h"
#include "../include/log.h"
#include "../include/MQTT.h"
#include <cjson/cJSON.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>

//#define ENABLE_DEBUG_MSG = y

/* GLOBAL VARIABLES */
BT_edge_event_info edge_event_info_BT_1; 

int main()
{
    /* Log configuration ================================= */
    const char *log_filePath = "/home/pi/Surveillance-System/Camera/src/Software_HM_Control/log/log_HM_Control.txt";
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
    /* Setting buttons and leds ============================== */
    if(!configureLed_bt())
        return EXIT_FAILURE;

    struct gpiod_line_request *buttons_request = getRequestBT();
    /* End settings buttons and leds ========================= */

    if(mqtt_init() != MQTTCLIENT_SUCCESS)
        return EXIT_FAILURE;

    mqtt_subscribe(MQTT_TOPIC_LED);

    /* Process button event triggered ========================= */
    struct       gpiod_edge_event_buffer *event_buffet_bt = gpiod_edge_event_buffer_new(BT_MAX_EVENT_BUFFER);
    struct       gpiod_edge_event *event;
    enum         gpiod_edge_event_type eventType = GPIOD_EDGE_EVENT_FALLING_EDGE;
    unsigned int eventOffset;
    int          n_events = 0;

    while(gpiod_line_request_wait_edge_events(buttons_request, -1))
    {
        n_events = gpiod_line_request_read_edge_events(buttons_request, event_buffet_bt, BT_MAX_EVENT_BUFFER);
        if(n_events != -1)
        {
            for(int event_index = 0; event_index < n_events; event_index++)
            {
                event       = gpiod_edge_event_buffer_get_event(event_buffet_bt, event_index);
                eventType   = gpiod_edge_event_get_event_type(event);
                eventOffset =  gpiod_edge_event_get_line_offset(event);

                if(eventType == GPIOD_EDGE_EVENT_RISING_EDGE)
                {
                    #ifdef ENABLE_DEBUG_MSG
                        log_debug("Number of events: %d | Event read: RISING | Offset: %d", n_events, eventOffset);
                    #endif
                    
                    clock_gettime(CLOCK_MONOTONIC, &edge_event_info_BT_1.edge_rising_timestamp);
                }
                else if(eventType == GPIOD_EDGE_EVENT_FALLING_EDGE)
                {
                #ifdef ENABLE_DEBUG_MSG
                    log_debug("Number of events: %d | Event read: FALLING | Offset: %d", n_events, eventOffset);
                #endif
                    clock_gettime(CLOCK_MONOTONIC, &edge_event_info_BT_1.edge_falling_timestamp);

                    char* bt_event_type = detect_BT_event_type(&edge_event_info_BT_1);
                    
                #ifdef ENABLE_DEBUG_MSG
                    log_debug("Button action detected: %s | Line: %d.", bt_event_type, eventOffset);
                #endif
                
                    cJSON *json_bt_action = cJSON_CreateObject();
                    cJSON_AddStringToObject(json_bt_action, "prs_type", bt_event_type);
                    if(eventOffset == BT_1_LINE)
                        cJSON_AddNumberToObject(json_bt_action, "bt_id", 1);
                    else if(eventOffset == BT_2_LINE)
                        cJSON_AddNumberToObject(json_bt_action, "bt_id", 2);

                    mqtt_publish(MQTT_TOPIC_BT, cJSON_Print(json_bt_action), false);
                }
            }
        }
    }

    /* End process button event triggered ======================= */
    return 0;
}

