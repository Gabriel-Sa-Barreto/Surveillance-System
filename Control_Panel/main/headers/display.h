#ifndef __DISPLAY_H
    #define __DISPLAY_H

    #include "ssd1306.h"
    #include "esp_log.h"
    #include <string.h>
    
    void renderingScreenInitial(int buttonID, int *menuPosition);
    void rederingConfirmationScreen(int buttonID, int *menuPosition, char *title);
    void rederingCamerasModeScreen(int buttonID, int *menuPosition);
    void rendering_ConfirmationScreen(char *msg);
    void rendering_alertScreen(char *cam_ID, char *can_Zone);
    void printText(int page, char *text, int text_len, bool invert);
    void printBitmap(int xpos, int ypos, uint8_t * bitmap, int width, int height, bool invert);
    void initOledDisplay();
    void cleanScreen();
    void cleanPage(int page);

#endif