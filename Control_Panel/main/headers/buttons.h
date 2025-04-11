#ifndef __BUTTONS_H
    #define __BUTTONS_H

    #include "general.h"
    #include "driver/gpio.h"
    #include <freertos/FreeRTOS.h>
    #include <freertos/task.h>
    
    void    set_button(uint8_t BT_PIN, bool en_pullUp, bool en_pullDown);
    void    set_logic_level(uint8_t en_level);
    uint8_t readButton(uint8_t BT_PIN);

#endif