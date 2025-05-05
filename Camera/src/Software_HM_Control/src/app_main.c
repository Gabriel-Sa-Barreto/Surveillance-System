#include "../include/gpio.h"
#include "../include/log.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <time.h>

#define NUM_BT 1
#define NUM_LED 2
#define BT_1_LINE 22
#define BT_2_LINE 23
#define BT_MAX_EVENT_BUFFER 2
#define LED_1_LINE 17
#define LED_2_LINE 27
#define LONG_PRESS_BUTTON_TIME 2 // 2 seconds

enum button_event_type { QUICK_PRESS, LONG_PRESS, NO_DETECTION };

typedef struct
{
    unsigned int BT_ID;
    struct timespec edge_rising_timestamp;
    struct timespec edge_falling_timestamp;
} BT_edge_event_info;

/* GLOBAL VARIABLES */
BT_edge_event_info edge_event_info_BT_1; 

/* FUNCTION PROTOTYPE */
unsigned int detect_event_type(BT_edge_event_info *edge_event_info_BT)
{
    time_t sec_diff = edge_event_info_BT->edge_falling_timestamp.tv_sec - edge_event_info_BT->edge_rising_timestamp.tv_sec;             
    long nsec_diff  = edge_event_info_BT->edge_falling_timestamp.tv_nsec - edge_event_info_BT->edge_rising_timestamp.tv_nsec;

    // Reset timestamp
    edge_event_info_BT->edge_falling_timestamp.tv_sec  = 0;
    edge_event_info_BT->edge_falling_timestamp.tv_nsec = 0;
    edge_event_info_BT->edge_rising_timestamp.tv_sec  = 0;
    edge_event_info_BT->edge_rising_timestamp.tv_nsec = 0;

    if((sec_diff + nsec_diff / 1e9) < LONG_PRESS_BUTTON_TIME)
        return QUICK_PRESS;
    else
        return LONG_PRESS;
}

int main()
{
    const char *chipPath     = "/dev/gpiochip0";
    const char *log_filePath = "/home/pi/Surveillance-System/Camera/src/Software_HM_Control/log/log_HM_Control.txt";
    struct gpiod_line_request *request_bt;
    struct gpiod_line_request *request_led;
    size_t MAX_BT_EVENTS = BT_MAX_EVENT_BUFFER;

    unsigned int offset_bt[NUM_BT]   = { BT_1_LINE };
    unsigned int offset_led[NUM_LED] = { LED_1_LINE, LED_2_LINE };

    struct stat buffer;
    bool logFile_exist = stat(log_filePath, &buffer) == 0 ? true : false;
    FILE *fp           = (logFile_exist) ? fopen(log_filePath, "a") : fopen(log_filePath, "w");

    /* Log configuration ================================= */
    if(!fp)
    {
        printf("Error to open file log.\n");
        return EXIT_FAILURE;
    }

    if(log_add_fp(fp, LOG_INFO) == -1)
    {
        printf("Error to add file log pointer.\n");
        return EXIT_FAILURE;
    }
    else
        log_info("Log system started.");
    /* End Log configuration ============================== */
    /* GPIO configuration ================================= */
    request_bt  = gpio_request_to_input(chipPath, offset_bt, NUM_BT, "set-bt", GPIOD_LINE_EDGE_BOTH, MAX_BT_EVENTS, true);
    request_led = gpio_request_to_output(chipPath, offset_led, NUM_LED, "set-led");

    if(!request_bt)
    {
        log_error("Button request failed.");
        return EXIT_FAILURE;
    }else
    {
        log_info("Button request successfully.");
        edge_event_info_BT_1.BT_ID = 1;
    }

    if(!request_led)
    {
        log_error("Led request failed.");
        return EXIT_FAILURE;
    }else
        log_info("Led request successfully.");
    /* End GPIO configuration ================================= */
    /* Process button event triggered ========================= */
    struct       gpiod_edge_event_buffer *event_buffet_bt = gpiod_edge_event_buffer_new(MAX_BT_EVENTS);
    struct       gpiod_edge_event *event;
    enum         gpiod_edge_event_type eventType = GPIOD_EDGE_EVENT_FALLING_EDGE;
    unsigned int eventOffset;
    int          n_events = 0;

    while(gpiod_line_request_wait_edge_events(request_bt, -1))
    {
        n_events = gpiod_line_request_read_edge_events(request_bt, event_buffet_bt, MAX_BT_EVENTS);
        if(n_events != -1)
        {
            for(int event_index = 0; event_index < n_events; event_index++)
            {
                event       = gpiod_edge_event_buffer_get_event(event_buffet_bt, event_index);
                eventType   = gpiod_edge_event_get_event_type(event);
                eventOffset =  gpiod_edge_event_get_line_offset(event);

                if(eventType == GPIOD_EDGE_EVENT_RISING_EDGE)
                {
                    log_debug("Number of events: %d | Event read: RISING | Offset: %d", n_events, eventOffset);
                    clock_gettime(CLOCK_MONOTONIC, &edge_event_info_BT_1.edge_rising_timestamp);
                }
                else if(eventType == GPIOD_EDGE_EVENT_FALLING_EDGE)
                {
                    log_debug("Number of events: %d | Event read: FALLING | Offset: %d", n_events, eventOffset);
                    clock_gettime(CLOCK_MONOTONIC, &edge_event_info_BT_1.edge_falling_timestamp);

                    enum button_event_type bt_event_type = detect_event_type(&edge_event_info_BT_1);
                    
                    if(bt_event_type == QUICK_PRESS)
                    {
                        log_debug("Quick press detected on button of line %d.", eventOffset);
                    }
                    else if(bt_event_type == LONG_PRESS)
                    {    
                        log_debug("Long press detected on button of line %d.", eventOffset);
                    }
                }
            }
        }else
            log_warn("Failed to read the button event.");
    }

    /* End process button event triggered ======================= */
    return 0;
}