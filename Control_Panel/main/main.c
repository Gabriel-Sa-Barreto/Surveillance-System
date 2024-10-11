#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/event_groups.h>
#include "freertos/queue.h"
#include "esp_system.h"
#include "esp_timer.h"
#include "spi_flash_mmap.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "connect.h"
#include "headers/MQTT.h"
#include "headers/general.h"
#include "headers/buttons.h"
#include "headers/display.h"

/* Variable that inform whether the system is activated */
bool isActivated_system;

/* Handler Variables */
QueueHandle_t      QueueBT;
QueueHandle_t      alertQueue;
esp_timer_handle_t xTimerHandler;
esp_timer_handle_t xTimerAlertLedHandler;
TaskHandle_t       mainTaskHandler;

uint8_t wifiLogo[] = {
    0b00000000, 0b00001110,
    0b00000000, 0b00001110,
    0b00000000, 0b11101110,
    0b00000000, 0b11101110,
    0b00001110, 0b11101110,
    0b00001110, 0b11101110,
    0b11101110, 0b11101110,
    0b11101110, 0b11101110
};
uint8_t wifi_failed[] = {
    0b10001000, 0b00001110,
    0b01010000, 0b00001010,
    0b00100000, 0b11101010,
    0b10010000, 0b10101010,
    0b00001110, 0b10101010,
    0b00001010, 0b10101010,
    0b11101010, 0b10101010,
    0b10101010, 0b10101010
};
uint8_t mqtt_symbol[] = {
    0b00000000, 0b00000000,
    0b00000000, 0b00000000,
    0b01111111, 0b11111110,
    0b01000000, 0b10101010,
    0b01000000, 0b00000010,
    0b01111111, 0b11111110,
    0b00011000, 0b00011000,
    0b00011000, 0b00011000
};
uint8_t mqtt_failed[] = {
    0b00001000, 0b00010000,
    0b00000100, 0b00100000,
    0b01111111, 0b11111110,
    0b01000001, 0b10101010,
    0b01000001, 0b10000010,
    0b01111111, 0b11111110,
    0b00000100, 0b00100000,
    0b00001000, 0b00010000
};

enum screen_state
{
    STATE_INITIAL_SCREEN,
    STATE_RENDERING_SCREEN,
    STATE_ENABLE_SYSTEM,
    STATE_DISABLE_SYSTEM,
    STATE_CAMERAS_MODE,
    STATE_CHECK_ALERT,
    STATE_PROCESS_ALERT
};

/* Prototype of the Functions */
void initTimers();
void initTimer_to_connectionStatus();
void alertSignal();
void info_systemConnectionStatus(void* arg);

void initTimers()
{
	const esp_timer_create_args_t periodic_timer_args = {
            .callback = &info_systemConnectionStatus,
            /* name is optional, but may help identify the timer when debugging */
            .name = TIMER_STATUS_TAG
    };

	ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &xTimerHandler));
	/* The timer has been created but is not running yet */
	
	/* Start the timers */
    ESP_ERROR_CHECK(esp_timer_start_periodic(xTimerHandler, 10000000));
	ESP_LOGI(TIMER_STATUS_TAG, "Status Timer started, time since boot: %lld us", esp_timer_get_time());
}

void initTimerAlertLed()
{
	const esp_timer_create_args_t periodic_timer_args = {
            .callback = &alertSignal,
            /* name is optional, but may help identify the timer when debugging */
            .name = TIMER_ALERT_TAG
    };

	ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &xTimerAlertLedHandler));
	/* The timer has been created but is not running yet */
	
	/* Start the timers */
    ESP_ERROR_CHECK(esp_timer_start_periodic(xTimerAlertLedHandler, 1000000));
	ESP_LOGI(TIMER_ALERT_TAG, "Alert Timer started, time since boot: %lld us", esp_timer_get_time());	
}

void alertSignal()
{
	static uint32_t statusLed;
	if(statusLed == 1)
		statusLed = 0;
	else
		statusLed = 1;
	gpio_set_level(LED_RED_PIN, statusLed);
}

void info_systemConnectionStatus(void* arg)
{
	ESP_LOGI(TIMER_STATUS_TAG, "Checking connection status....");
	if(wifi_isConnected())
	{ 
		printBitmap(0, 0, wifiLogo, 16, 8, false);
		if(mqtt_connected())
		{
			printBitmap((8*3) - 1, 0, mqtt_symbol, 16, 8, false);
		}else
		{
			printBitmap((8*3) - 1, 0, mqtt_failed, 16, 8, false);
		}
	}else
	{
		printBitmap(0, 0, wifi_failed, 16, 8, false);
		printBitmap((8*3) - 1, 0, mqtt_failed, 16, 8, false);
		wifi_connect();
	}
}

bool send_msgTo_enableSystem(char *enable)
{
	char msg[25];
	uint32_t response = 0;
	if(!mqtt_connected())
	{
		return false;
	}
	strcpy(msg, "{\"enable_system\":");
	strcat(msg, enable);
	strcat(msg, "}");
	mqtt_publish(TOPIC_EN_OR_DIS_SYSTEM, msg);
	BaseType_t xResult = xTaskNotifyWait(0, 0, &response, pdMS_TO_TICKS(5000));
	if(xResult == pdFAIL)
	{
		return false;
	}
	return true;
}

bool send_msgTo_camMode(int cam_mode)
{
	char msg[20];
	char mode[2];
	uint32_t response = 0;
	if(!mqtt_connected())
	{
		return false;
	}
	sprintf(mode, "%d", cam_mode);
	strcpy(msg, "{\"cam_mode\":");
	strcat(msg, mode);
	strcat(msg, "}");
	mqtt_publish(TOPIC_CAM_MODE, msg);
	BaseType_t xResult = xTaskNotifyWait(0, 0, &response, pdMS_TO_TICKS(5000));
	if(xResult == pdFAIL)
	{
		return false;
	}
	return true;
}

void task_Main(void *args)
{
	//SCREEN CONTROL AND USER ACTIONS
	int buttonID       = 0;
	int state          = STATE_RENDERING_SCREEN;
	int screenID       = 0;
	
	int menuPosition_screen_0 = 0;
	int menuPosition_screen_1 = 0;
	int menuPosition_screen_2 = 0;
	int menuPosition_screen_3 = 0;
	char *confirmationText = "";
	alertData alert;
	isActivated_system = false;
	while (true)
	{	
		switch (state)
		{
			case STATE_RENDERING_SCREEN:
				cleanPage(1);
				cleanPage(2);
				cleanPage(3);
				if(screenID == 0)
				{
					renderingScreenInitial(buttonID, &menuPosition_screen_0);
					state        = STATE_INITIAL_SCREEN;
				}
				else if(screenID == 1)
				{  
					rederingConfirmationScreen(buttonID, &menuPosition_screen_1, "Enable System:");
					state        = STATE_ENABLE_SYSTEM;

				}
				else if(screenID == 2)
				{  
					rederingConfirmationScreen(buttonID, &menuPosition_screen_2, "Disable System:");
					state        = STATE_DISABLE_SYSTEM;
				}
				else if(screenID == 3)
				{  
					rederingCamerasModeScreen(buttonID, &menuPosition_screen_3);
					state        = STATE_CAMERAS_MODE;
				}
				else if(screenID == 4)
				{  
					rendering_ConfirmationScreen(confirmationText);
					state        = STATE_RENDERING_SCREEN;
					screenID     = 0;
				}else if(screenID == 5)
				{
					printText(1, "Sys. is already", 16, false);
					printText(2, confirmationText, 16, false);
					vTaskDelay(pdMS_TO_TICKS(2000));
					state        = STATE_RENDERING_SCREEN;
					screenID     = 0;
				}else if(screenID == 6)
				{
					{
						char camID[10];
						sprintf(camID, "%d", alert.cameraID);
						rendering_alertScreen(camID, alert.zone);
					}
					state = STATE_PROCESS_ALERT;
				}
				vTaskDelay(pdMS_TO_TICKS(100));
				break;
			case STATE_INITIAL_SCREEN:
				if(xQueueReceive(QueueBT, &buttonID, pdMS_TO_TICKS(100)))
				{
					if(buttonID != 1)
					{
						state    = STATE_RENDERING_SCREEN;
						screenID = 0;
					}else
					{
						if(menuPosition_screen_0 == 0) // Enable system
						{
							state = STATE_RENDERING_SCREEN;
							if(!isActivated_system)
							{
								screenID = 1;								
							}else
							{
								screenID = 5;
								confirmationText = "    enabled !   ";
							}

						}
						else if(menuPosition_screen_0 == 1) // Disable system
						{ 
							state = STATE_RENDERING_SCREEN;
							if(isActivated_system)
							{
								screenID = 2;
							}else
							{
								screenID = 5;
								confirmationText = "  deactivated ! ";
							}
						}
						else if(menuPosition_screen_0 == 2) { screenID = 3; state = STATE_RENDERING_SCREEN; }
						menuPosition_screen_0 = 0;
					}
				}else
				{
					if(xQueueReceive(alertQueue, &alert, pdMS_TO_TICKS(0)))
					{
						initTimerAlertLed();
						state    = STATE_RENDERING_SCREEN;
						screenID = 6;
					}else
					{
						state    = STATE_INITIAL_SCREEN;
					}
				}
				break;
			case STATE_ENABLE_SYSTEM:
				if(xQueueReceive(QueueBT, &buttonID, pdMS_TO_TICKS(100)))
				{
					if(buttonID != 1)
					{
						state    = STATE_RENDERING_SCREEN;
						screenID = 1;
					}else
					{
						if(menuPosition_screen_1 == 0) // (YES)
						{
							screenID = 4;
							if(send_msgTo_enableSystem("true"))
							{
								isActivated_system = true;
								gpio_set_level(LED_GREEN_PIN, 1);
								gpio_set_level(LED_RED_PIN,   0);
								confirmationText = "  -> Confirmed  ";
							}else
							{
								isActivated_system = false;
								gpio_set_level(LED_GREEN_PIN, 0);
								gpio_set_level(LED_RED_PIN,   1);
								confirmationText = "    -> Error    ";
							}
						} 
						else if(menuPosition_screen_1 == 1) // (NO)
						{ 
							screenID = 0;
						} 
						menuPosition_screen_1 = 0;
						state                 = STATE_RENDERING_SCREEN;
					}
				}else
				{
					state = STATE_ENABLE_SYSTEM;
				}
				break;
			case STATE_DISABLE_SYSTEM:
				if(xQueueReceive(QueueBT, &buttonID, pdMS_TO_TICKS(100)))
				{
					if(buttonID != 1)
					{
						state    = STATE_RENDERING_SCREEN;
						screenID = 2;
					}else
					{
						if(menuPosition_screen_2 == 0) // (YES)
						{
							screenID = 4;
							if(send_msgTo_enableSystem("false"))
							{
								isActivated_system = false;
								gpio_set_level(LED_GREEN_PIN, 0);
								gpio_set_level(LED_RED_PIN,   1);
								confirmationText = "  -> Confirmed  ";
							}else
							{
								isActivated_system = true;
								gpio_set_level(LED_GREEN_PIN, 1);
								gpio_set_level(LED_RED_PIN,   0);
								confirmationText = "    -> Error    ";
							}
						} 
						else if(menuPosition_screen_2 == 1) // (NO)
						{
							screenID = 0;
						}
						menuPosition_screen_2 = 0;
						state                 = STATE_RENDERING_SCREEN;
					}
				}else
				{
					if(xQueueReceive(alertQueue, &alert, pdMS_TO_TICKS(0)))
					{
						initTimerAlertLed();
						state    = STATE_RENDERING_SCREEN;
						screenID = 6;
					}else
					{
						state    = STATE_DISABLE_SYSTEM;
					}
				}
				break;
			case STATE_CAMERAS_MODE:
				if(xQueueReceive(QueueBT, &buttonID, pdMS_TO_TICKS(100)))
				{
					if(buttonID != 1)
					{
						state    = STATE_RENDERING_SCREEN;
						screenID = 3;
					}else
					{
						if(menuPosition_screen_3 == 0) // (Continuos)
						{
							screenID = 4;
							if(isActivated_system)
							{
								if(send_msgTo_camMode(1))
								{
									confirmationText = "  -> Confirmed  ";
								}else
								{
									confirmationText = "    -> Error    ";
								}
							}else
							{
								confirmationText = "    -> Error    ";
							}
						} 
						else if(menuPosition_screen_3 == 1) // (Personalized)
						{
							screenID = 4;
							if(isActivated_system)
							{
								if(send_msgTo_camMode(2))
								{
									confirmationText = "  -> Confirmed  ";
								}else
								{
									confirmationText = "    -> Error    ";
								}
							}else
							{
								confirmationText = "    -> Error    ";
							}					
						}
						else if(menuPosition_screen_3 == 2) { screenID = 0; } // (Back)
						menuPosition_screen_3 = 0;
						state                 = STATE_RENDERING_SCREEN;
					}
				}else
				{
					if(xQueueReceive(alertQueue, &alert, pdMS_TO_TICKS(0)))
					{
						initTimerAlertLed();
						state    = STATE_RENDERING_SCREEN;
						screenID = 6;
					}else
					{
						state    = STATE_CAMERAS_MODE;
					}
				}
				break;
			case STATE_PROCESS_ALERT:
				if(xQueueReceive(QueueBT, &buttonID, pdMS_TO_TICKS(100)))
				{
					if(buttonID == 1)
					{
						esp_timer_stop(xTimerAlertLedHandler);
						gpio_set_level(LED_RED_PIN, 0);
						state            = STATE_RENDERING_SCREEN;
						screenID         = 4;
						confirmationText = "  -> Confirmed  ";
					}
				}else
				{
					if(xQueueReceive(alertQueue, &alert, pdMS_TO_TICKS(0)))
					{
						state    = STATE_RENDERING_SCREEN;
						screenID = 6;
					}else
					{
						state    = STATE_PROCESS_ALERT;
					}
				}			    
				break;
			default:
				break;
		}
	}
}

void app_main(void)
{
	// CONFIGURE OLED DISPLAY
	initOledDisplay();
	cleanScreen();
	printText(0, "Setup:----------", 16, false);
	printText(1, "(OK) Display", 16, false);
	// ================================================================
	// CONFIGURE PUSH BUTTONS
	initButtons();
	QueueBT = getHandlerQueueBT();
	printText(2, "(OK) Buttons", 16, false);
	// ================================================================
	// CONFIGURE LEDS
	gpio_set_direction(LED_GREEN_PIN, GPIO_MODE_OUTPUT);
	gpio_set_direction(LED_RED_PIN,   GPIO_MODE_OUTPUT);
	gpio_set_level(LED_GREEN_PIN, 0);
	gpio_set_level(LED_RED_PIN,   1);
	// ================================================================
	alertQueue = xQueueCreate(4, sizeof(alertData));
	// ================================================================
	// CONFIGURE WIFI
	esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
	{
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    ESP_LOGI(WIFI_TAG, "ESP_WIFI_MODE_STA");
	esp_err_t isWifiOnline = initWifi(); 
    if(isWifiOnline == ESP_OK)
	{
		printText(3, "(OK) Wifi", 9, false);
	}else
	{
		printText(3, "(ER) Wifi", 9, false);
	}
	// WAIT UNTIL CONNECT
	while(!wifi_isConnected())
	{
		vTaskDelay(pdTICKS_TO_MS(1000));
	}
	cleanScreen();
	// ================================================================
	// CREATE TASK TO CONTROL THE SCREEN AND USER ACTIONS
	xTaskCreate(task_Main, "Main Task", 9*4096, NULL, 1, &mainTaskHandler);
	// ================================================================
	// CONFIGURE MQTT --------------------------------------------------
	// MQTT go running on second Esp32 Core
	mqttParameter *mqttParam = (mqttParameter *) malloc(sizeof(mqttParameter));
	mqttParam->alertQueue      = alertQueue;
	mqttParam->mainTaskHandler = mainTaskHandler;
	xTaskCreatePinnedToCore(task_MQTT_Controller, "Task MQTT", 9*4096, (void *) mqttParam, 1, get_mqttHandle(), 1);
	mqtt_connection();
	// ================================================================
	info_systemConnectionStatus((void*) NULL);
	// INIT TIMER ------------------------------------------------------
	initTimers();
	//------------------------------------------------------------------
}
