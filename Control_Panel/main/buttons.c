#include "headers/buttons.h"

uint8_t enable_level;

uint8_t readButton(uint8_t BT_PIN)
{
	uint8_t value = gpio_get_level(BT_PIN);
	if(value == enable_level)
	{
		while(gpio_get_level(BT_PIN) == enable_level) //Loop to wait the button to be released.
		{
			vTaskDelay(pdMS_TO_TICKS(50));
		}
	}
	return value;
}

void set_button(uint8_t BT_PIN, bool en_pullUp, bool en_pullDown)
{
	gpio_set_direction(BT_PIN, GPIO_MODE_INPUT);
	if(en_pullUp)
		gpio_pullup_en(BT_PIN);
	else
		gpio_pullup_dis(BT_PIN);

	if(en_pullDown)
		gpio_pulldown_en(BT_PIN);
	else
		gpio_pulldown_dis(BT_PIN);
}

void set_logic_level(uint8_t en_level)
{
	enable_level = en_level;
}