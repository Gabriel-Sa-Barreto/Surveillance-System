#include "headers/displayOled.h"

SSD1306_t oledDisplay;

void printText(int page, char *text, int text_len, bool invert)
{

	ssd1306_display_text(&oledDisplay, page, text, text_len, invert);

}

void printText_x3(int page, char *text, int text_len, bool invert)
{

	ssd1306_display_text_x3(&oledDisplay, page, text, text_len, invert);

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

void cleanPage_range(int first, int last)
{
	for(int i = first; i <= last; i++)
		ssd1306_clear_line(&oledDisplay, i, false);
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
	ssd1306_init(&oledDisplay, DISPLAY_COL, DISPLAY_LINE);
#endif // CONFIG_SSD1306_128x64
#if CONFIG_SSD1306_128x32
	ESP_LOGI(DISPLAY_TAG, "Oled Display is 128x32");
	ssd1306_init(&oledDisplay, DISPLAY_COL, DISPLAY_LINE);
#endif // CONFIG_SSD1306_128x32
    cleanScreen();
    ssd1306_contrast(&oledDisplay, 0xff);
}