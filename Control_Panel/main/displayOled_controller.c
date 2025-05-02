#include "headers/displayOled_controller.h"
#include "headers/general.h"

#define WIFI_LOGO_LINE  8
#define WIFI_LOGO_COL  16

/* Global Variables */
uint8_t cursorPosition = 0;
QueueHandle_t queueHandler_Alertzones;

uint8_t bitmap_wifiConnected[] = {
    0b00000000, 0b00001110,
    0b00000000, 0b00001110,
    0b00000000, 0b11101110,
    0b00000000, 0b11101110,
    0b00001110, 0b11101110,
    0b00001110, 0b11101110,
    0b11101110, 0b11101110,
    0b11101110, 0b11101110
};
uint8_t bitmap_wifi_disconnected[] = {
    0b10001000, 0b00001110,
    0b01010000, 0b00001010,
    0b00100000, 0b11101010,
    0b10010000, 0b10101010,
    0b00001110, 0b10101010,
    0b00001010, 0b10101010,
    0b11101010, 0b10101010,
    0b10101010, 0b10101010
};

uint8_t bitmap_SCREEN_CURSOR[] = {
    0b00000000, 0b11000000,
    0b00000001, 0b11100000,
    0b00000001, 0b11111100,
    0b11111111, 0b11111111,
    0b11111111, 0b11111111,
    0b00000001, 0b11111100,
    0b00000001, 0b11100000,
    0b00000000, 0b11000000
};

uint8_t bitmap_erase_SCREEN_CURSOR[] = {
    0b00000000, 0b00000000,
    0b00000000, 0b00000000,
    0b00000000, 0b00000000,
    0b00000000, 0b00000000,
    0b00000000, 0b00000000,
    0b00000000, 0b00000000,
    0b00000000, 0b00000000,
    0b00000000, 0b00000000
};

uint8_t bitmap_confirmation_symbol[] = {
    0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
    0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
    0b00000000, 0b00000111, 0b11111111, 0b00000000, 0b00000000,
    0b00000000, 0b00011100, 0b00000000, 0b11100000, 0b00000000,
    0b00000000, 0b01100000, 0b11111000, 0b00110000, 0b00000000,
    0b00000000, 0b11000111, 0b00000111, 0b00001000, 0b00000000,
    0b00000001, 0b10011000, 0b00000000, 0b11000100, 0b00000000,
    0b00000010, 0b00110000, 0b00000000, 0b00110010, 0b00000000,
    0b00000110, 0b01000000, 0b00000000, 0b00011001, 0b00000000,
    0b00000100, 0b10000000, 0b00000000, 0b00001000, 0b10000000,
    0b00001001, 0b10000000, 0b00000000, 0b01000100, 0b10000000,
    0b00001001, 0b00000000, 0b00000001, 0b10100110, 0b11000000,
    0b00011011, 0b00000000, 0b00000011, 0b00100010, 0b01000000,
    0b00010010, 0b00000000, 0b00000110, 0b01000010, 0b01000000,
    0b00010010, 0b00000000, 0b00001100, 0b10000011, 0b01000000,
    0b00010010, 0b00011000, 0b00011001, 0b00000001, 0b01100000,
    0b00010010, 0b00110100, 0b00110010, 0b00000001, 0b00100000,
    0b00010010, 0b00110010, 0b01100100, 0b00000001, 0b00100000,
    0b00010010, 0b00011001, 0b11011000, 0b00000001, 0b01000000,
    0b00010010, 0b00001100, 0b10110000, 0b00000010, 0b01000000,
    0b00011011, 0b00000110, 0b01100000, 0b00000010, 0b01000000,
    0b00001001, 0b00000011, 0b11000000, 0b00000110, 0b01000000,
    0b00001001, 0b10000000, 0b10000000, 0b00000100, 0b10000000,
    0b00000100, 0b10000000, 0b00000000, 0b00001100, 0b10000000,
    0b00000110, 0b01000000, 0b00000000, 0b00011001, 0b00000000,
    0b00000010, 0b00100000, 0b00000000, 0b00110011, 0b00000000,
    0b00000001, 0b00011000, 0b00000000, 0b01100110, 0b00000000,
    0b00000000, 0b10000111, 0b00000011, 0b10001100, 0b00000000,
    0b00000000, 0b01100001, 0b11111100, 0b00010000, 0b00000000,
    0b00000000, 0b00011000, 0b00000000, 0b11100000, 0b00000000,
    0b00000000, 0b00000111, 0b10001111, 0b10000000, 0b00000000,
    0b00000000, 0b00000000, 0b01111000, 0b00000000, 0b00000000
};

uint8_t bitmap_error_symbol[] = {
    0b00000000, 0b00000111, 0b11100000, 0b00000000, 
    0b00000000, 0b00111111, 0b11111100, 0b00000000, 
    0b00000000, 0b11111111, 0b11111111, 0b00000000, 
    0b00000011, 0b11111000, 0b00011111, 0b11000000, 
    0b00000111, 0b11100000, 0b00000111, 0b11100000, 
    0b00001111, 0b10000000, 0b00000001, 0b11110000, 
    0b00011110, 0b00000000, 0b00000000, 0b11111000, 
    0b00011110, 0b00000000, 0b00000000, 0b01111000, 
    0b00111100, 0b00000000, 0b00000000, 0b00111100, 
    0b00111000, 0b00000000, 0b00000000, 0b00011100, 
    0b01110000, 0b00011000, 0b00011000, 0b00011110, 
    0b01110000, 0b00111100, 0b00111100, 0b00001110, 
    0b01110000, 0b00111110, 0b01111100, 0b00001110, 
    0b11100000, 0b00011111, 0b11111000, 0b00001111, 
    0b11100000, 0b00001111, 0b11110000, 0b00000111, 
    0b11100000, 0b00000111, 0b11100000, 0b00000111, 
    0b11100000, 0b00000111, 0b11100000, 0b00000111, 
    0b11100000, 0b00001111, 0b11110000, 0b00000111, 
    0b11100000, 0b00011111, 0b11111000, 0b00001111, 
    0b01110000, 0b00111110, 0b01111000, 0b00001110, 
    0b01110000, 0b00111100, 0b00111100, 0b00001110, 
    0b01110000, 0b00011000, 0b00011000, 0b00001110, 
    0b00111000, 0b00000000, 0b00000000, 0b00011100, 
    0b00111100, 0b00000000, 0b00000000, 0b00111100, 
    0b00011100, 0b00000000, 0b00000000, 0b00111000, 
    0b00011110, 0b00000000, 0b00000000, 0b01111000, 
    0b00001111, 0b10000000, 0b00000001, 0b11110000, 
    0b00000111, 0b11000000, 0b00000011, 0b11100000, 
    0b00000011, 0b11111000, 0b00011111, 0b11000000, 
    0b00000000, 0b11111111, 0b11111111, 0b00000000, 
    0b00000000, 0b00111111, 0b11111100, 0b00000000, 
    0b00000000, 0b00000111, 0b11100000, 0b00000000, 
};

uint16_t DisplayTask_format_screenChange_command(uint8_t buttonPressed, uint8_t screens_ID)
{
    uint16_t command = 0;
    command   = command | buttonPressed;
    command <<= 3;
    command   = command | screens_ID;
    command <<= 2;
    command   = command | CHANGE_SCREEN;
    return command;
}

uint16_t DisplayTask_format_sucess_failureScreen_command(uint8_t screens_ID)
{
    uint16_t command = 0;
    command   = command | screens_ID;
    command <<= 2;
    command   = command | CHANGE_SCREEN;
    return command;
}

uint8_t getCursorPosition(void)
{
    return cursorPosition;
}

void setCursorPosition(uint8_t value)
{
    cursorPosition = value;
}

void renderConectionLogo(bool status)
{
    if(status)
        printBitmap(0, 0, bitmap_wifiConnected, WIFI_LOGO_COL, WIFI_LOGO_LINE, false);
    else
        printBitmap(0, 0, bitmap_wifi_disconnected, WIFI_LOGO_COL, WIFI_LOGO_LINE, false);
}

void updateTopMenu(void)
{
    bool wifi_status;
    bool mqtt_status;
    bool checkConnection;
    char ptr_time[8];
    char space[18] = "         \0";
    getTime(ptr_time, sizeof(ptr_time));
    ESP_LOGI(TIMER_TAG, "Current Time: %s", ptr_time);
    strcat(space, ptr_time); // align time to right side of menu

    wifi_status     = wifi_isConnected();
    mqtt_status     = mqtt_connected();
    checkConnection = !(!wifi_status || !mqtt_status);
    printText(D_PAGE_0,  space, strlen(space), false);
    renderConectionLogo( checkConnection );
}

void displayOled_task(void *args)
{
   SemaphoreHandle_t *xSemaphore_sincWithMain_task = (SemaphoreHandle_t *) args; 
   
   uint8_t  opcode                 =  0;
   uint8_t  data_button_pressed    =  0;
   uint8_t  data_screenID          = -1;
   uint8_t  total_menuOptions      =  0;
   uint16_t command                =  0;
   uint8_t  last_screen            = -1;
   uint8_t  last_cursor_position   =  0;
   while(queueHandler_Alertzones == NULL)
   {
       QueueHandle_t *handler   = get_queueHandler_alert();
       queueHandler_Alertzones  = *handler;
   }

   while (1)
   {
        // Waiting to receive a command.
        xTaskNotifyWait(0, 0, &command, portMAX_DELAY);
        opcode = 0b0000000011 & command;
        switch (opcode)
        {
            case CHANGE_SCREEN:
                last_screen            = data_screenID;
                data_screenID          = (0b00011100 & command) >> 2;
                data_button_pressed    = (0b01100000 & command) >> 5;
                break;
            default:          
                data_screenID          = -1;
                data_button_pressed    =  0;
                break;
        }

        switch (data_screenID)
        {
            case SCREEN_INITIAL_SCREEN: total_menuOptions = 3; break;
            case SCREEN_EN_SYSTEM:      total_menuOptions = 2; break;
            case SCREEN_DIS_SYSTEM:     total_menuOptions = 2; break;
            case SCREEN_CAMERAS_MODE:   total_menuOptions = 3; break;
            default:                    total_menuOptions = 0; break;
        }

        last_cursor_position = cursorPosition;
        if(data_button_pressed == BUTTON_UP)
        {
            cursorPosition -= 1;
            cursorPosition = (cursorPosition == 255) ? total_menuOptions - 1 : cursorPosition;

        } else if(data_button_pressed == BUTTON_DOWN)
        {
            cursorPosition += 1;
            cursorPosition = (cursorPosition == total_menuOptions) ? 0 : cursorPosition;
        }
        ESP_LOGI(DISPLAY_TAG, "Screen_ID: %d | Button: %d | Cursor: %d", data_screenID, data_button_pressed, cursorPosition);

        /* Render the screen */
        char *title   = " ";
        char *text_1  = " ";
        char *text_2  = " ";
        char *text_3  = " ";;
        switch (data_screenID)
        {
            case SCREEN_INITIAL_SCREEN:
                if(last_screen != data_screenID)
                {
                    cleanPage_range(D_PAGE_1, D_PAGE_6);
                    title  = "    * MENU *    ";
                    text_1 = "  Enable System";
                    text_2 = "  Disable System";
                    text_3 = "  Cam Mode";
                    printText(D_PAGE_2,  title, strlen(title), false);
                    printText(D_PAGE_3, text_1, strlen(text_1), false);
                    printText(D_PAGE_4, text_2, strlen(text_2), false);
                    printText(D_PAGE_5, text_3, strlen(text_3), false);
                }else // refresh only cursor's position
                {
                    printBitmap(0, (last_cursor_position + 3) * D_FONT_SIZE_LINE, bitmap_erase_SCREEN_CURSOR, BITMAP_SCREEN_CURSOR_COL, BITMAP_SCREEN_CURSOR_LINE, false);
                }
                printBitmap(0, (cursorPosition + 3) * D_FONT_SIZE_LINE, bitmap_SCREEN_CURSOR, BITMAP_SCREEN_CURSOR_COL, BITMAP_SCREEN_CURSOR_LINE, false);
                ESP_LOGI(DISPLAY_TAG, "Initial screen rendered.");
                xSemaphoreGive(*xSemaphore_sincWithMain_task);
                break;
            case SCREEN_EN_SYSTEM:
            case SCREEN_DIS_SYSTEM:
                if(last_screen != data_screenID)
                {
                    cleanPage_range(D_PAGE_1, D_PAGE_6);
                    text_1 = "  (YES)";
                    text_2 = "  (NO)";
                    if(data_screenID == SCREEN_EN_SYSTEM)
                        title = "*ENABLE SYSTEM*";
                    else
                        title = "*DISABLE SYSTEM*";
                    printText(D_PAGE_2, title,  strlen(title),  false);
                    printText(D_PAGE_4, text_1, strlen(text_1), false);
                    printText(D_PAGE_5, text_2, strlen(text_2), false);
                }else // refresh only cursor's position
                {
                    if(last_cursor_position == 0)
                        printBitmap(0, D_FONT_SIZE_LINE * D_PAGE_4, bitmap_erase_SCREEN_CURSOR, BITMAP_SCREEN_CURSOR_COL, BITMAP_SCREEN_CURSOR_LINE, false);
                    else
                        printBitmap(0, D_FONT_SIZE_LINE * D_PAGE_5, bitmap_erase_SCREEN_CURSOR, BITMAP_SCREEN_CURSOR_COL, BITMAP_SCREEN_CURSOR_LINE, false);
                }
                if(cursorPosition == 0) // option - YES
                    printBitmap(0, D_FONT_SIZE_LINE * D_PAGE_4, bitmap_SCREEN_CURSOR, BITMAP_SCREEN_CURSOR_COL, BITMAP_SCREEN_CURSOR_LINE, false);
                else                    // option - NO            
                    printBitmap(0, D_FONT_SIZE_LINE * D_PAGE_5, bitmap_SCREEN_CURSOR, BITMAP_SCREEN_CURSOR_COL, BITMAP_SCREEN_CURSOR_LINE, false);
                ESP_LOGI(DISPLAY_TAG, "Screen rendered.");
                xSemaphoreGive(*xSemaphore_sincWithMain_task);
                break;
            case SCREEN_CAMERAS_MODE:
                if(last_screen != data_screenID)
                {
                    cleanPage_range(D_PAGE_1, D_PAGE_6);
                    title  = "  * CAM MODE *  ";
                    text_1 = "  Continuous";
                    text_2 = "  Personalized";
                    text_3 = "  <- Back";
                    printText(D_PAGE_2, title,  strlen(title),  false);
                    printText(D_PAGE_4, text_1, strlen(text_1), false);
                    printText(D_PAGE_5, text_2, strlen(text_2), false);
                    printText(D_PAGE_6, text_3, strlen(text_3), false);
                }else // refresh only cursor's position
                {
                    printBitmap(0, (last_cursor_position + 4) * D_FONT_SIZE_LINE, bitmap_erase_SCREEN_CURSOR, BITMAP_SCREEN_CURSOR_COL, BITMAP_SCREEN_CURSOR_LINE, false);
                }
                printBitmap(0, (cursorPosition + 4) * D_FONT_SIZE_LINE, bitmap_SCREEN_CURSOR, BITMAP_SCREEN_CURSOR_COL, BITMAP_SCREEN_CURSOR_LINE, false);
                ESP_LOGI(DISPLAY_TAG, "Cam mode screen rendered.");
                xSemaphoreGive(*xSemaphore_sincWithMain_task);
                break;
            case SCREEN_ALERT_PROCESS: 
                cleanPage_range(D_PAGE_1, D_PAGE_7);
                title  = " == ! ALERT ! ==";
                text_1 = "      Zones     ";
                text_2 = "    Disable    ";
                printText(D_PAGE_2, title,  strlen(title),  false);
                printText(D_PAGE_3, text_1, strlen(text_1), false);
                printText(D_PAGE_7, text_2, strlen(text_2), false);
                printBitmap(D_FONT_SIZE_COL * 2, D_PAGE_7 * D_FONT_SIZE_LINE, bitmap_SCREEN_CURSOR, BITMAP_SCREEN_CURSOR_COL, BITMAP_SCREEN_CURSOR_LINE, false);
                xSemaphoreGive(*xSemaphore_sincWithMain_task);
                {
                    int qtd_zones = 0;
                    int index     = 0;
                    char stop     = ' ';
                    char **matrix = (char **) malloc(sizeof(char *));
                    char *zone    = (char *) malloc(ZONE_NAME_SIZE * sizeof(char));
                    while(stop != '*')
                    {
                        if(xQueueReceive(queueHandler_Alertzones, zone, 0))
                        {
                            matrix[qtd_zones] = (char *) malloc(strlen(zone) * sizeof(char));
                            sprintf(matrix[qtd_zones], "%s", zone);
                            qtd_zones++;
                        }
                        cleanPage(D_PAGE_5);
                        printText(D_PAGE_5, matrix[index],  strlen(matrix[index]),  false);
                        if(++index == qtd_zones)
                            index = 0;
                        if(xTaskNotifyWait(0, 0, &stop, 0))
                        {
                            if(stop == '*')
                            {
                                xSemaphoreGive(*xSemaphore_sincWithMain_task);
                                break;
                            }
                        }else
                            vTaskDelay(pdMS_TO_TICKS(1000));
                    }
                    free(matrix);
                    free(zone);
                }
                break;
            case SCREEN_VALIDATE_PWD:
                {
                    cleanPage_range(D_PAGE_1, D_PAGE_7);
                    uint32_t char_received;
                    char    *password;
                    char     char_pwd;
                    uint8_t  n_char = 0;
                    title    = "====PASSWORD====";
                    password = (char *) malloc(PASSWORD_SIZE * sizeof(char));             
                    printText(D_PAGE_2, title, strlen(title), false);
                    printBitmap(0, D_PAGE_4 * D_FONT_SIZE_LINE, bitmap_SCREEN_CURSOR, BITMAP_SCREEN_CURSOR_COL, BITMAP_SCREEN_CURSOR_LINE, false);
                    xSemaphoreGive(*xSemaphore_sincWithMain_task);
                    while (xTaskNotifyWait(0, 0, &char_received, portMAX_DELAY))
                    {
                        char_pwd = (char) char_received;
                        n_char++;
                        /* Receives a char belonging to the password */
                        if(char_pwd != '*') // DIFFERENT OF STOP OPERATION
                        {
                            if(char_pwd != '#') // DIFFERENT OF ERASE PASSWORD
                            {
                                if(n_char <= PASSWORD_SIZE)
                                {
                                    ESP_LOGI(DISPLAY_TAG, "Char Received: %c", char_pwd);
                                    password[n_char - 1] = char_pwd;
                                    printText_x3(D_PAGE_4, password, n_char, false);
                                }
                            }else
                            {   
                                // Clear the password and reinitilize.
                                password = (char *) malloc(PASSWORD_SIZE * sizeof(char));
                                n_char   = 0;
                                cleanPage(D_PAGE_4);
                                cleanPage(D_PAGE_5);
                                cleanPage(D_PAGE_6);
                            }
                            // Releases the semaphore to the main task
                            xSemaphoreGive(*xSemaphore_sincWithMain_task);
                        }else
                        {
                            xSemaphoreGive(*xSemaphore_sincWithMain_task);
                            break;
                        }
                    }
                }
                break;
            case SCREEN_OPERATION_FAILURE:
            case SCREEN_OPERATION_SUCCESS:
                cleanPage_range(D_PAGE_3, D_PAGE_7);
                if(data_screenID == SCREEN_OPERATION_FAILURE)
                    printBitmap((DISPLAY_COL/2) - (BITMAP_ERROR_COL/2), D_PAGE_3 * D_FONT_SIZE_LINE, bitmap_error_symbol, BITMAP_ERROR_COL, BITMAP_ERROR_LINE, false);
                else
                    printBitmap((DISPLAY_COL/2) - (BITMAP_CONFIRMATION_COL/2), D_PAGE_3 * D_FONT_SIZE_LINE, bitmap_confirmation_symbol, BITMAP_CONFIRMATION_COL, BITMAP_CONFIRMATION_LINE, false);
                vTaskDelay(pdMS_TO_TICKS(3000));
                xSemaphoreGive(*xSemaphore_sincWithMain_task);
                break;
            default:
                break;
        }
   }
}