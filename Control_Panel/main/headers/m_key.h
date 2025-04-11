#ifndef __M_KEY_H
    #define __M_KEY_H
    #include "general.h"
    #include "esp_log.h"
    #include "driver/gpio.h"
    #include <freertos/FreeRTOS.h>
    #include <freertos/task.h>

    void init_m_key();
    /* 
        Returns the position of the pressed key. 
        FAILED: Returns -1.    
    */
    int  scan_keyboard();
    /* 
       Returns the char related to key.
       FAILED: Null. 
    */
    char key_selected(uint8_t position);
#endif
