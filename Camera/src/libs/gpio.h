#ifndef __GPIO_H
    #define __GPIO_H
    #include <gpiod.h>
    #include "string.h"
    #include "stdio.h"
    #include "log.h"

    #define GPIO_DIRECTION_IN "in"
    #define GPIO_DIRECTION_OUT "out"
    
    /* Request a line as input */
    struct gpiod_line_request* gpio_request_to_input(const char   *chip_path,
                                                     unsigned int *offset,
                                                     int           num_lines,
                                                     const char   *consumer,
                                                     enum gpiod_line_edge edgeEvent,
                                                     size_t        event_buffer_size,
                                                     bool          setDebounce);
    /* Request a line as output */
    struct gpiod_line_request* gpio_request_to_output(const char  *chip_path,
                                                     unsigned int *offset,
                                                     int           num_lines,
                                                     const char   *consumer);
    int  gpio_read(struct gpiod_line_request *request, const unsigned int offset);
    void gpio_write(struct gpiod_line_request *request, const unsigned int offset, enum gpiod_line_value value);
    void gpio_release_request(struct gpiod_line_request *request);

    #endif