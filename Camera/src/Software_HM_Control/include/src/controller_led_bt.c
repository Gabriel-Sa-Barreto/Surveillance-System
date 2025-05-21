#include "../controller_led_bt.h"

enum leds_mode { LED_OFF, LED_ON, LED_BLINK};
enum leds_ID   { LED_1 = 1, LED_2 };

unsigned int offset_bt[NUM_BT]   = { BT_1_LINE, BT_2_LINE };
unsigned int offset_led[NUM_LED] = { LED_1_LINE, LED_2_LINE };

const char *chipPath  = "/dev/gpiochip0";
struct gpiod_line_request *request_led;
struct gpiod_line_request *request_bt;
bool timerLed_enabled = false;

static void blinkLed_1(int sig, siginfo_t *sigInfo, void *vParam)
{
    static enum gpiod_line_value led_1_status = GPIOD_LINE_VALUE_INACTIVE;
    if(sig == TIMER_SIG1)
    {
        led_1_status = (led_1_status == GPIOD_LINE_VALUE_INACTIVE) ? GPIOD_LINE_VALUE_ACTIVE : GPIOD_LINE_VALUE_INACTIVE;
        gpiod_line_request_set_value(request_led, LED_1_LINE, led_1_status);
    }
}

static void blinkLed_2(int sig, siginfo_t *sigInfo, void *vParam)
{
    static enum gpiod_line_value led_2_status = GPIOD_LINE_VALUE_INACTIVE;
    if(sig == TIMER_SIG1)
    {
        led_2_status = (led_2_status == GPIOD_LINE_VALUE_INACTIVE) ? GPIOD_LINE_VALUE_ACTIVE : GPIOD_LINE_VALUE_INACTIVE;
        gpiod_line_request_set_value(request_led, LED_2_LINE, led_2_status);
    }
}

static void setLedMode(int led_line, int mode)
{
    switch(mode)
    {
        case LED_OFF:
            if(timerLed_enabled)
            {
                timerLed_enabled = false;
                stop_timer(); 
            }
            gpiod_line_request_set_value(request_led, led_line, GPIOD_LINE_VALUE_INACTIVE);
            break;
        case LED_ON:
            if(timerLed_enabled)
            {
                timerLed_enabled = false;
                stop_timer(); 
            }
            gpiod_line_request_set_value(request_led, led_line, GPIOD_LINE_VALUE_ACTIVE);
            break;
        case LED_BLINK:
            if(timerLed_enabled)
                stop_timer();
            else
                timerLed_enabled = true;
            if(led_line == LED_1_LINE)
                set_timer(LED_DELAY_TIMER, LED_DELAY_INTERVAL, blinkLed_1);
            else
                set_timer(LED_DELAY_TIMER, LED_DELAY_INTERVAL, blinkLed_2);
            break;
        default: break;
    }
}

void setLeds(int id, int mode)
{
    switch(id)
    {
        case LED_1: setLedMode(LED_1_LINE, mode); break;
        case LED_2: setLedMode(LED_2_LINE, mode); break;
        default: break;
    }
}

bool configureLed_bt()
{
    request_bt  = gpio_request_to_input(chipPath, offset_bt, NUM_BT, "set-bt", GPIOD_LINE_EDGE_BOTH, BT_MAX_EVENT_BUFFER, true);
    request_led = gpio_request_to_output(chipPath, offset_led, NUM_LED, "set-led");

    if(!request_bt)
    {
        log_error("Button request failed.");
        return false;
    }else
    {
        log_info("Button request successfully.");
    }

    if(!request_led)
    {
        log_error("Led request failed.");
        return false;
    }else
        log_info("Led request successfully.");

    return true;
}

char* detect_BT_event_type(BT_edge_event_info *edge_event_info_BT)
{
    time_t sec_diff = edge_event_info_BT->edge_falling_timestamp.tv_sec - edge_event_info_BT->edge_rising_timestamp.tv_sec;             
    long nsec_diff  = edge_event_info_BT->edge_falling_timestamp.tv_nsec - edge_event_info_BT->edge_rising_timestamp.tv_nsec;

    // Reset timestamp
    edge_event_info_BT->edge_falling_timestamp.tv_sec  = 0;
    edge_event_info_BT->edge_falling_timestamp.tv_nsec = 0;
    edge_event_info_BT->edge_rising_timestamp.tv_sec   = 0;
    edge_event_info_BT->edge_rising_timestamp.tv_nsec  = 0;

    if((sec_diff + nsec_diff / 1e9) < LONG_PRESS_BUTTON_TIME)
        return BT_EVENT_QUICK_PRESS;
    else
        return BT_EVENT_LONG_PRESS;
}

struct gpiod_line_request* getRequestBT(void)
{
    return request_bt;
}