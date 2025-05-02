#ifndef __GPIO_H
    #define __GPIO_H
    #include "gpiod.h"
    #include "string.h"
    #include "stdio.h"

    #define GPIO_DIRECTION_IN "in"
    #define GPIO_DIRECTION_OUT "out"

    #define TAG "[GPIO CONTROL]"
    
    /* Request a line as input or output */
    struct gpiod_line_request *gpio_request_lines(const char *chip_path, unsigned int offset, const char *consumer, char *in_out);
    int  gpio_read(gpiod_line_request *request, const unsigned int offset);
    void gpio_write(gpiod_line_request *request, const unsigned int offset, enum gpiod_line_value value);
    void gpio_release_request(gpiod_line_request *request);

    #endif