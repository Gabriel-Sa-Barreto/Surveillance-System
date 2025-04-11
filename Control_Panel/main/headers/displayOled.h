#ifndef __DISPLAY_OLED_H
    #define __DISPLAY_OLED_H
    #include <freertos/FreeRTOS.h>
    #include "freertos/semphr.h"
    #include "ssd1306.h"
    #include "esp_log.h"
    #include <string.h>
    #include "headers/general.h"
    
    void printText(int page, char *text, int text_len, bool invert);
    void printText_x3(int page, char *text, int text_len, bool invert);
    void printBitmap(int xpos, int ypos, uint8_t * bitmap, int width, int height, bool invert);
    void initOledDisplay();
    void cleanScreen();
    void cleanPage(int page);
    void cleanPage_range(int first, int last);
#endif