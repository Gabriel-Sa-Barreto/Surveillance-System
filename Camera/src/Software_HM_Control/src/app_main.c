#include "headers/gpio.h"
#include <errno.h>

const unsigned int LED_1_LINE 17;
const unsigned int LED_2_LINE 27;
const unsigned int BT_1_LINE 22;

int main(void)
{
    const char *chipPath;
    struct gpiod_line_request request_leds;
    struct gpiod_line_request request_bt;

    const unsigned int offset_leds[] = { LED_1_LINE, LED_2_LINE };
    const unsigned int offset_bt[]   = { BT_1_LINE };

    request_leds = gpio_request_lines(chip_path, offset_leds, "set-leds", GPIO_DIRECTION_OUT);
    request_bt   = gpio_request_lines(chip_path, offset_bt,   "set-bt", GPIO_DIRECTION_IN);

    if(!request_leds)
    {
        fprintf(stderr, "%s Failed to request line to Leds: %s\n", TAG, strerror(errno));
        return EXIT_FAILURE;
    }

    if(!request_bt)
    {
        fprintf(stderr, "%s Failed to request line to Butttons: %s\n", TAG, strerror(errno));
        return EXIT_FAILURE;
    }

    while(true)
    {
        value = (value == GPIOD_LINE_VALUE_ACTIVE) ? GPIOD_LINE_VALUE_INACTIVE : GPIOD_LINE_VALUE_ACTIVE;
        gpio_write(request_leds, LED_1_LINE, value);
        sleep(1);
    }

    return 0;
}