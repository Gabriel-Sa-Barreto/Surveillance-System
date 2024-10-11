#include "headers/buttons.h"

QueueHandle_t  interruptQueueBT;
QueueHandle_t      valuesQueueBT;

static void IRAM_ATTR BT_1_Interrupt(void *params)
{
	int bt1_value = 1;
	xQueueSendFromISR(interruptQueueBT, &bt1_value, NULL);
}

static void IRAM_ATTR BT_2_Interrupt(void *params)
{
	int bt2_value = 2;
	xQueueSendFromISR(interruptQueueBT, &bt2_value, NULL);
}

static void IRAM_ATTR BT_3_Interrupt(void *params)
{
	int bt3_value = 3;
	xQueueSendFromISR(interruptQueueBT, &bt3_value, NULL);
}

QueueHandle_t getHandlerQueueBT()
{
    return valuesQueueBT;
}

void task_BTSignalDebounce(void *params)
{
	int BT_ID;
	int last_BT_ID = -1;
	while(1){
		if(xQueueReceive(interruptQueueBT, &BT_ID, pdMS_TO_TICKS(150)))
		{
			last_BT_ID = BT_ID;
		}else
		{
			if(last_BT_ID != -1)
			{
				ESP_LOGI("BT", "Button %d pressed.", last_BT_ID);
				xQueueSend(valuesQueueBT, &last_BT_ID, pdMS_TO_TICKS(0));
				last_BT_ID = -1;
			}
		}
	}
}

void initButtons()
{
    gpio_set_direction(BT_1_PIN, GPIO_MODE_INPUT);
	gpio_set_direction(BT_2_PIN, GPIO_MODE_INPUT);
	gpio_set_direction(BT_3_PIN, GPIO_MODE_INPUT);
	// Disables pulldown and pullup
	gpio_pulldown_dis(BT_1_PIN);
	gpio_pulldown_dis(BT_2_PIN);
	gpio_pulldown_dis(BT_3_PIN);
	gpio_pullup_dis(BT_1_PIN);
	gpio_pullup_dis(BT_2_PIN);
	gpio_pullup_dis(BT_3_PIN);
	gpio_set_intr_type(BT_1_PIN, GPIO_INTR_POSEDGE);
	gpio_set_intr_type(BT_2_PIN, GPIO_INTR_POSEDGE);
	gpio_set_intr_type(BT_3_PIN, GPIO_INTR_POSEDGE);

    interruptQueueBT = xQueueCreate(4, sizeof(int));
	valuesQueueBT    = xQueueCreate(4, sizeof(int));

    xTaskCreate(task_BTSignalDebounce, "Button_SignalDebounce", 2048, NULL, 1, NULL);

    gpio_install_isr_service(0);
	gpio_isr_handler_add(BT_1_PIN, BT_1_Interrupt, (void *)BT_1_PIN);
	gpio_isr_handler_add(BT_2_PIN, BT_2_Interrupt, (void *)BT_2_PIN);
	gpio_isr_handler_add(BT_3_PIN, BT_3_Interrupt, (void *)BT_3_PIN);
}