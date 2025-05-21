#ifndef __CONTROLLER_LED_BT_H
    #define __CONTROLLER_LED_BT_H

    #include "../include/gpio.h"
    #include "../include/log.h"
    #include "../include/Timer.h"
    #include <time.h>
    #include <signal.h>
    #include <stdlib.h>

    #define LED_DELAY_TIMER 500     // Time in miliseconds
    #define LED_DELAY_INTERVAL 500  // Time in miliseconds

    #define LED_1_LINE 17
    #define LED_2_LINE 27
    #define BT_1_LINE 22
    #define BT_2_LINE 23

    #define BT_EVENT_NO_DETECTION "none"
    #define BT_EVENT_QUICK_PRESS "quick"
    #define BT_EVENT_LONG_PRESS "long"

    #define NUM_BT 2
    #define NUM_LED 2
    #define BT_MAX_EVENT_BUFFER 2
    #define LONG_PRESS_BUTTON_TIME 2 // 2 seconds

    typedef struct
    {
        unsigned int BT_ID;
        struct timespec edge_rising_timestamp;
        struct timespec edge_falling_timestamp;
    } BT_edge_event_info;

    void setLeds(int id, int mode);
    bool configureLed_bt();
    char* detect_BT_event_type(BT_edge_event_info *edge_event_info_BT);
    struct gpiod_line_request* getRequestBT(void);

#endif