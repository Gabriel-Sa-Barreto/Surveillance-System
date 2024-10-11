#include "headers/Display.h"

#define DISPLAY_TAG "SSD1306"

SSD1306_t oledDisplay;

uint8_t screenCursor[] = {
    0b00000000, 0b11000000,
    0b00000001, 0b10100000,
    0b00000001, 0b10010000,
    0b11111111, 0b10000100,
    0b11111111, 0b10000100,
    0b00000001, 0b10010000,
    0b00000001, 0b10100000,
    0b00000000, 0b11000000
};

void renderingScreenInitial(int buttonID, int *menuPosition)
{
	int totalOptions = 3;
	//ssd1306_clear_line(&oledDisplay, *menuPosition + 1, false);
	if(buttonID == 3) // UP
	{
		*menuPosition -= 1;
		*menuPosition = (*menuPosition < 0) ? totalOptions-1 : *menuPosition;
	}else if(buttonID == 2) // Down
	{
		*menuPosition += 1;
		*menuPosition = (*menuPosition == totalOptions) ? 0 : *menuPosition;
	}
	//ssd1306_clear_line(&oledDisplay, *menuPosition + 1, false);
	printText(1, "   En.System",  12,  false);
	printText(2, "   Dis.System", 13, false);
	printText(3, "   Mode",       7,  false);
	printBitmap(0, (*menuPosition + 1) * 8, screenCursor, 16, 8, false);
	ESP_LOGD("SCREEN", "Initial screen refreshed.");
}

void rederingConfirmationScreen(int buttonID, int *menuPosition, char *title)
{
	int totalOptions = 2;
	//ssd1306_clear_line(&oledDisplay, *menuPosition + 1, false);
	if(buttonID == 3) // UP
	{
		*menuPosition -= 1;
		*menuPosition = (*menuPosition < 0) ? totalOptions-1 : *menuPosition;
	}else if(buttonID == 2) // Down
	{
		*menuPosition += 1;
		*menuPosition = (*menuPosition == totalOptions) ? 0 : *menuPosition;
	}
	//ssd1306_clear_line(&oledDisplay, 2, false);
	printText(1, title,  14,  false);
	printText(2, "   (YES)   (NO)", 15, false);
	if(*menuPosition == 0)
		printBitmap(0, 16, screenCursor, 16, 8, false);
	else
		printBitmap(8*8, 16, screenCursor, 16, 8, false);
	ESP_LOGD("SCREEN", "Confirmation Screen refreshed.");
}

void rederingCamerasModeScreen(int buttonID, int *menuPosition)
{
	int totalOptions = 3;
	//ssd1306_clear_line(&oledDisplay, *menuPosition + 1, false);
	if(buttonID == 3) // UP
	{
		*menuPosition -= 1;
		*menuPosition = (*menuPosition < 0) ? totalOptions-1 : *menuPosition;
	}else if(buttonID == 2) // Down
	{
		*menuPosition += 1;
		*menuPosition = (*menuPosition == totalOptions) ? 0 : *menuPosition;
	}
	printText(1, "   Continuous",   13,  false);
	printText(2, "   Personalized", 15, false);
	printText(3, "   <-",           5,  false);
	printBitmap(0, (*menuPosition + 1) * 8, screenCursor, 16, 8, false);
	ESP_LOGD("SCREEN", "Initial screen refreshed.");
}

void rendering_ConfirmationScreen(char *msg)
{
	ssd1306_display_text(&oledDisplay, 2, msg, 16, false);
	vTaskDelay(pdMS_TO_TICKS(2000));
}

void rendering_alertScreen(char *cam_ID, char *can_Zone)
{
	char ID[16];
	char zone[16];
	strcpy(ID, "Camera ID:");
	strcat(ID, cam_ID);
	strcpy(zone, "Zone:");
	strcat(zone, can_Zone);
	printText(1, ID,   16,  false);
	printText(2, zone,   16,  false);
	printText(3, "    (Disable)   ",   16,  false);
	printBitmap(8, 3*8, screenCursor, 16, 8, false);
}

void printText(int page, char *text, int text_len, bool invert)
{
    ssd1306_display_text(&oledDisplay, page, text, text_len, invert);
}

void printBitmap(int xpos, int ypos, uint8_t * bitmap, int width, int height, bool invert)
{
    ssd1306_bitmaps(&oledDisplay, xpos, ypos, bitmap, width, height, invert);
}

void cleanScreen()
{
    ssd1306_clear_screen(&oledDisplay, false);
}

void cleanPage(int page)
{
    ssd1306_clear_line(&oledDisplay, page, false);
}

void initOledDisplay()
{

#if CONFIG_I2C_INTERFACE
	ESP_LOGI(DISPLAY_TAG, "INTERFACE is i2c");
	ESP_LOGI(DISPLAY_TAG, "CONFIG_SDA_GPIO=%d",CONFIG_SDA_GPIO);
	ESP_LOGI(DISPLAY_TAG, "CONFIG_SCL_GPIO=%d",CONFIG_SCL_GPIO);
	ESP_LOGI(DISPLAY_TAG, "CONFIG_RESET_GPIO=%d",CONFIG_RESET_GPIO);
	i2c_master_init(&oledDisplay, CONFIG_SDA_GPIO, CONFIG_SCL_GPIO, CONFIG_RESET_GPIO);
#endif // CONFIG_I2C_INTERFACE

#if CONFIG_SSD1306_128x64
	ESP_LOGI(DISPLAY_TAG, "Panel is 128x64");
	ssd1306_init(&oledDisplay, 128, 64);
#endif // CONFIG_SSD1306_128x64
#if CONFIG_SSD1306_128x32
	ESP_LOGI(DISPLAY_TAG, "Oled Display is 128x32");
	ssd1306_init(&oledDisplay, 128, 32);
#endif // CONFIG_SSD1306_128x32
    cleanScreen();
    ssd1306_contrast(&oledDisplay, 0xff);
}