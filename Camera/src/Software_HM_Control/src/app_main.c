#include "../include/gpio.h"
#include <unistd.h>
#include <stdlib.h>

int main()
{
    const char *chipPath = "/dev/gpiochip0";
    struct gpiod_line_request *request_leds;
    struct gpiod_line_request *request_bt;
    enum gpiod_line_value value  = GPIOD_LINE_VALUE_ACTIVE;
    int BT_value = GPIOD_LINE_VALUE_ACTIVE;

    unsigned int LED_1_LINE = 17;
    unsigned int LED_2_LINE = 27;
    unsigned int BT_1_LINE  = 22;

    unsigned int offset_leds[2] = { LED_1_LINE, LED_2_LINE };
    unsigned int offset_bt[1]   = { BT_1_LINE };

    request_leds = gpio_request_to_lines(chipPath, offset_leds, 2, "set-leds", GPIO_DIRECTION_OUT);
    request_bt   = gpio_request_to_lines(chipPath, offset_bt,   1, "set-bt", GPIO_DIRECTION_IN);

    if(!request_leds)
    {
        printf("%s Failed to request line to Leds:\n", TAG);
        return EXIT_FAILURE;
    }

    if(!request_bt)
    {
        printf("%s Failed to request line to Butttons: %s\n", TAG);
        return EXIT_FAILURE;
    }

    while(true)
    {
        value = (value == GPIOD_LINE_VALUE_ACTIVE) ? GPIOD_LINE_VALUE_INACTIVE : GPIOD_LINE_VALUE_ACTIVE;
        gpio_write(request_leds, LED_1_LINE, value);
        gpio_write(request_leds, LED_2_LINE, value);

        BT_value = gpio_read(request_bt, offset_bt[0]);
        printf("%s Button value: %d\n", TAG, BT_value);
        sleep(1);
    }

    return 0;
}