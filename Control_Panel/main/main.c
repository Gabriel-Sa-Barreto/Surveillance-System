#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "freertos/semphr.h"
#include "esp_system.h"
#include "esp_timer.h"
#include "esp_log.h"
#include "spi_flash_mmap.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include "../components/wifi/connect.h"
#include "headers/buttons.h"
#include "headers/m_key.h"
#include "headers/displayOled.h"
#include "headers/displayOled_controller.h"
#include "headers/project_Timers.h"
#include "headers/MQTT.h"
#include "headers/packageProcessor.h"
#include "headers/general.h"
#include "headers/fileSystem.h"
enum states
{
	STATE_INITIAL_SCREEN,
	STATE_EN_SYSTEM,
	STATE_DIS_SYSTEM,
	STATE_CAMERAS_MODE,
	STATE_PROCESS_ALERT,
	STATE_VALIDATE_PWD,
	STATE_PROCESS_OPERATION,
	STATE_REFRESH_PWD,
	STATE_OPERATION_SUCCESS,
	STATE_OPERATION_FAILURE
};

enum initial_screen_menu_option
{
	OPTION_ENABLE_SYSTEM,
	OPTION_DISABLE_SYSTEM,
	OPTION_CAM_MODE
};

enum cam_mode_option
{
	OPTION_CONTINUOUS_MODE,
	OPTION_PERSONALIZED_MODE,
	OPTION_TO_BACK,
};

enum confirmationOptions
{	
	OPTION_YES,
	OPTION_NO
};

enum opcode_actions
{
	OP_EN_SYSTEM,  /* ENABLE SYTEM      */
	OP_DIS_SYSTEM, /* DISABLE SYSTEM    */
	OP_CONT_MODE,  /* CONTINUOUS MODE   */
	OP_PERS_MODE,  /* PERSONALIZED MODE */
	OP_REF_PWD,    /* REFRESH PASSWORD  */
	OP_DIS_ALERT   /* DISABLE ALERT     */
};

/* Prototypes of the functions */
void    main_task(void *args);
void    sendCommand_to_refreshScreen(uint8_t buttonPressed, uint8_t screens_ID);
uint8_t getButtonPressed();
bool    validatePassword(char *password);
bool    process_current_operation(uint8_t action);

/* Handler Variables */
TaskHandle_t      taskHandler_main;
TaskHandle_t      taskHandler_displayOled;
TaskHandle_t      taskHandler_pkg_processor;
QueueHandle_t     queueHandler_receivedAlerts;
SemaphoreHandle_t semaphore_sinc_TaskDisplayAndMain;

void app_main(void)
{
	// CONFIGURE PUSH BUTTONS
	set_logic_level(1);
	set_button(BT_1_PIN, false, false);
	set_button(BT_2_PIN, false, false);
	set_button(BT_3_PIN, false, false);
	// ================================================================
	// CONFIGURE LEDS
	gpio_set_direction(LED_GREEN_PIN, GPIO_MODE_OUTPUT);
	gpio_set_direction(LED_RED_PIN,   GPIO_MODE_OUTPUT);
	gpio_set_level(LED_GREEN_PIN, 0);
	gpio_set_level(LED_RED_PIN,   1);
	// ================================================================
	// CONFIGURE MATRIX KEYBOARD
	init_m_key();
	// ================================================================
	// CONFIGURE DISPLAY
	initOledDisplay();
	cleanScreen();
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
		while(!wifi_isConnected()) { vTaskDelay(pdTICKS_TO_MS(1000)); }
	}else
		esp_restart();
	// ================================================================
	// CREATES TASKS
	semaphore_sinc_TaskDisplayAndMain = xSemaphoreCreateBinary();
	xTaskCreatePinnedToCore(task_MQTT_Controller,   "mqtt_task",    3*4096, NULL, PR_MQTT_TASK, get_mqttHandle(), 1);
	xTaskCreatePinnedToCore(displayOled_task,      "display_task",  7*4096, (void *) semaphore_sinc_TaskDisplayAndMain, PR_DISPLAY_TASK, &taskHandler_displayOled, 0);
	xTaskCreatePinnedToCore(task_packageProcessor, "pkg_processor", 3*4096, (void *) getHandler_MQTTpackageQueue(), PR_PKG_PROCESSOR_TASK, &taskHandler_pkg_processor, 0);
	xTaskCreatePinnedToCore(main_task,             "main_task",     9*4096, NULL, PR_MAIN_TASK, &taskHandler_main, 0);
	// INIT TIMER
	initTimer_rfs_top_menu();
	// ================================================================
	mqtt_connection();
	// ================================================================
	// INIIT THE LITTLE_FS
	if(!initFileSystem())
		esp_restart();
	// ================================================================
	updateTopMenu();
}

void main_task(void *args)
{
	uint8_t next_state        =  STATE_INITIAL_SCREEN;
	uint8_t previous_state    = -1;
	uint8_t current_action    =  0;
	uint8_t buttonPressed     =  0;
	uint8_t next_screenID     = -1;
	uint8_t current_screen    = -1;
	uint8_t sys_status        =  0; // 0 - disabled | 1 - enabled  

	while(queueHandler_receivedAlerts == NULL)
	{
		QueueHandle_t *handler   = get_queueHandler_alert();
		queueHandler_receivedAlerts      = *handler;
	}
	
	// Sends command to display task
	sendCommand_to_refreshScreen(0, SCREEN_INITIAL_SCREEN);
	while (true)
	{
		vTaskDelay(pdMS_TO_TICKS(100));
		switch (next_state)
		{
			case STATE_INITIAL_SCREEN:
				buttonPressed = getButtonPressed();
				if(buttonPressed == BUTTON_UP || buttonPressed == BUTTON_DOWN)
				{
					sendCommand_to_refreshScreen(buttonPressed, SCREEN_INITIAL_SCREEN);
				}else if(buttonPressed == BUTTON_ENTER)
				{
					if(getCursorPosition() == OPTION_ENABLE_SYSTEM && sys_status == 0)
					{
						next_state    = STATE_EN_SYSTEM;
						next_screenID = SCREEN_EN_SYSTEM;
						setCursorPosition(0);			
					}
					else if(getCursorPosition() == OPTION_DISABLE_SYSTEM && sys_status == 1)
					{
						next_state    = STATE_DIS_SYSTEM;
						next_screenID = SCREEN_DIS_SYSTEM;
						setCursorPosition(0);
					}
					else if(getCursorPosition() == OPTION_CAM_MODE && sys_status == 1)
					{
						next_state    = STATE_CAMERAS_MODE;
						next_screenID = SCREEN_CAMERAS_MODE;
						setCursorPosition(0);
					}else
					{
						next_state    = STATE_INITIAL_SCREEN;
						next_screenID = SCREEN_INITIAL_SCREEN; 
					}
					sendCommand_to_refreshScreen(0, next_screenID);
				}else if(uxQueueMessagesWaiting(queueHandler_receivedAlerts) > 0)
				{
					// New alert packages need to be process.
					next_state    = STATE_PROCESS_ALERT;
					setCursorPosition(0);
					sendCommand_to_refreshScreen(0, SCREEN_ALERT_PROCESS);
				}else
				{
					next_state = STATE_INITIAL_SCREEN;
				}
				break;
			case STATE_EN_SYSTEM:
			case STATE_DIS_SYSTEM:
				buttonPressed = getButtonPressed();
				current_screen = (next_state == STATE_EN_SYSTEM) ? SCREEN_EN_SYSTEM : SCREEN_DIS_SYSTEM;
				current_action = (next_state == STATE_EN_SYSTEM) ? OP_EN_SYSTEM : OP_DIS_SYSTEM;
				if(buttonPressed == BUTTON_UP || buttonPressed == BUTTON_DOWN)
				{
					sendCommand_to_refreshScreen(buttonPressed, current_screen);
				}else if(buttonPressed == BUTTON_ENTER)
				{
					if(getCursorPosition() == OPTION_YES)
					{
						next_state    = STATE_VALIDATE_PWD;
						next_screenID = SCREEN_VALIDATE_PWD;				
					}
					else if(getCursorPosition() == OPTION_NO)
					{
						next_state    = STATE_INITIAL_SCREEN;
						next_screenID = SCREEN_INITIAL_SCREEN;
					}
					setCursorPosition(0);
					sendCommand_to_refreshScreen(0, next_screenID);
				}else
				{
					next_state = (next_state == STATE_EN_SYSTEM) ? STATE_EN_SYSTEM : STATE_DIS_SYSTEM;
				}
				break;
			case STATE_CAMERAS_MODE:
				buttonPressed = getButtonPressed();
				if(buttonPressed == BUTTON_UP || buttonPressed == BUTTON_DOWN)
				{
					sendCommand_to_refreshScreen(buttonPressed, SCREEN_CAMERAS_MODE);
				}else if(buttonPressed == BUTTON_ENTER)
				{
					if(getCursorPosition() == OPTION_CONTINUOUS_MODE || getCursorPosition() == OPTION_PERSONALIZED_MODE)
					{
						next_state    = STATE_VALIDATE_PWD;
						next_screenID = SCREEN_VALIDATE_PWD;				
					}
					else if(getCursorPosition() == OPTION_TO_BACK)
					{
						next_state    = STATE_INITIAL_SCREEN;
						next_screenID = SCREEN_INITIAL_SCREEN;						
					}
					current_action = (getCursorPosition() == OPTION_CONTINUOUS_MODE) ? OP_CONT_MODE : OP_PERS_MODE;
					setCursorPosition(0);
					sendCommand_to_refreshScreen(0, next_screenID);
				}else
				{
					next_state = STATE_CAMERAS_MODE;
				}
				break;
			case STATE_PROCESS_ALERT:
				buttonPressed = getButtonPressed();
				if(buttonPressed == BUTTON_ENTER)
				{
					next_state     = STATE_VALIDATE_PWD;
					previous_state = STATE_PROCESS_ALERT;
					next_screenID  = SCREEN_VALIDATE_PWD;
					current_action = OP_DIS_ALERT;
					xTaskNotify(taskHandler_displayOled,(uint32_t) '*', eSetValueWithOverwrite);
					xSemaphoreTake(semaphore_sinc_TaskDisplayAndMain, portMAX_DELAY);
					sendCommand_to_refreshScreen(0, SCREEN_VALIDATE_PWD);
				}
				break;
			case STATE_VALIDATE_PWD:
				{	
					char  char_inserted;
					int   pos_key;
					char  password[PASSWORD_SIZE + 1];
					memset(password, ' ', (PASSWORD_SIZE + 1) * sizeof(char));
					uint8_t pwd_index = 0;
					while(next_state == STATE_VALIDATE_PWD)
					{
						pos_key        = scan_keyboard();
						buttonPressed  = getButtonPressed();
						if(pos_key != -1)
						{
							char_inserted = key_selected(pos_key);
							if(char_inserted == '*' && previous_state == STATE_PROCESS_ALERT) // STOP OPERATION
							{
								continue;
							}
							ESP_LOGI(APP_MAIN_TAG, "Char Inserted: %c", char_inserted);
							xTaskNotify(taskHandler_displayOled,(uint32_t) char_inserted, eSetValueWithOverwrite);
							// Waits until the display task to process the sent caractere.
							xSemaphoreTake(semaphore_sinc_TaskDisplayAndMain, portMAX_DELAY);
							if(char_inserted == '#') // ERASE THE PASSWORD
							{
                    	        memset(password, ' ', (PASSWORD_SIZE + 1) * sizeof(char));
								pwd_index = 0;
							}
							else if(char_inserted == '*') // STOP OPERATION
							{
								next_state     = STATE_INITIAL_SCREEN;
								previous_state = -1;
								sendCommand_to_refreshScreen(0, SCREEN_INITIAL_SCREEN);
							}else
							{
								if(pwd_index < PASSWORD_SIZE)
							    {
							    	password[pwd_index++] = char_inserted;
							    }
							}
						}else if(buttonPressed == BUTTON_ENTER)
						{
							password[PASSWORD_SIZE] = '\0';
							// VALIDATES THE PASSWORD
							if(validatePassword(password))
							{
								next_state     = STATE_PROCESS_OPERATION;
								previous_state = -1;
								char_inserted  = '*';
							}else
							{
								char_inserted = '#';
								pwd_index     = 0;
								memset(password, ' ', (PASSWORD_SIZE + 1) * sizeof(char));
							}
							xTaskNotify(taskHandler_displayOled,(uint32_t) char_inserted, eSetValueWithOverwrite);
							// Waits until the display task to process the sent caractere.
							xSemaphoreTake(semaphore_sinc_TaskDisplayAndMain, portMAX_DELAY);
						}
						vTaskDelay(pdMS_TO_TICKS(100));
					}
					break;
				}
			case STATE_PROCESS_OPERATION:
				if(process_current_operation(current_action))
				{
					next_state = STATE_OPERATION_SUCCESS;
					if(current_action == OP_EN_SYSTEM)
						sys_status = 1;
					else if(current_action == OP_DIS_SYSTEM)
						sys_status = 0;
				}else
					next_state = STATE_OPERATION_FAILURE;
				break;
			case STATE_REFRESH_PWD:
				break;
			case STATE_OPERATION_SUCCESS:
				next_state = STATE_INITIAL_SCREEN;
				sendCommand_to_refreshScreen(0, SCREEN_OPERATION_SUCCESS);
				sendCommand_to_refreshScreen(0, SCREEN_INITIAL_SCREEN);
				break;
			case STATE_OPERATION_FAILURE:
				next_state = STATE_INITIAL_SCREEN;
				sendCommand_to_refreshScreen(0, SCREEN_OPERATION_FAILURE);
				sendCommand_to_refreshScreen(0, SCREEN_INITIAL_SCREEN);
				break;
		}	
	}
}

uint8_t getButtonPressed()
{
	uint8_t BT_values = 0b000;
	BT_values = (readButton(BT_1_PIN)) ? BT_values | (1 << 2) : BT_values & ~(1 << 2);
	BT_values = (readButton(BT_2_PIN)) ? BT_values | (1 << 1) : BT_values & ~(1 << 1);
	BT_values = (readButton(BT_3_PIN)) ? BT_values | (1 << 0) : BT_values & ~(1 << 0);
	
	switch (BT_values)
	{
		case 0b001: ESP_LOGI(APP_MAIN_TAG, "Button pressed: %d", BUTTON_UP);    return BUTTON_UP;    break;
		case 0b010: ESP_LOGI(APP_MAIN_TAG, "Button pressed: %d", BUTTON_DOWN);  return BUTTON_DOWN;  break;
		case 0b100: ESP_LOGI(APP_MAIN_TAG, "Button pressed: %d", BUTTON_ENTER); return BUTTON_ENTER; break;
		default:    return 0;            break;
	}
}

bool validatePassword(char *password)
{
	ESP_LOGI(APP_MAIN_TAG, "Validating password: %s", password);
	// Loads the system's password
	char  system_pwd[PASSWORD_SIZE + 1];
	FILE *pwd_file = openFile(PWD_PATH, "r");
	if(pwd_file != NULL)
	{
        fgets(system_pwd, PASSWORD_SIZE + 1, pwd_file);
		ESP_LOGI(APP_MAIN_TAG, "System's Password: %s", system_pwd);
		fclose(pwd_file);
    }
	if(strcmp(system_pwd, password) != 0)
		return false;
	return true;
}

void sendCommand_to_refreshScreen(uint8_t buttonPressed, uint8_t screens_ID)
{
	uint8_t command = 0;
	switch (screens_ID)
	{
		case SCREEN_INITIAL_SCREEN:
		case SCREEN_EN_SYSTEM:
		case SCREEN_DIS_SYSTEM:
		case SCREEN_CAMERAS_MODE:
		case SCREEN_ALERT_PROCESS:      
		case SCREEN_VALIDATE_PWD:
			command = DisplayTask_format_screenChange_command(buttonPressed, screens_ID);
			break;
		case SCREEN_OPERATION_SUCCESS:
		case SCREEN_OPERATION_FAILURE:
			command = DisplayTask_format_sucess_failureScreen_command(screens_ID);
			break;
		default: break;
	}
	xTaskNotify(taskHandler_displayOled, command, eSetValueWithOverwrite);
	// Waits until the screen to be refreshed.
	xSemaphoreTake(semaphore_sinc_TaskDisplayAndMain, portMAX_DELAY);
}

bool process_current_operation(uint8_t action)
{
	bool operation_status;
	char msg[25];
	if(!wifi_isConnected() || !mqtt_connected())
	{
		ESP_LOGW(APP_MAIN_TAG, "Wifi or MQTT disconnected.");
		return false;
	}
	switch (action)
	{
		case OP_EN_SYSTEM:
			strcpy(msg, "{\"enable_system\":true}");
			if(mqtt_publish(TOPIC_EN_OR_DIS_SYSTEM, msg))
			{
				gpio_set_level(LED_GREEN_PIN, 1);
				gpio_set_level(LED_RED_PIN,   0);
				ESP_LOGI(APP_MAIN_TAG, "System activated!!");
				operation_status = true;
			}else
				operation_status = false;
			break;
		case OP_DIS_SYSTEM:
			strcpy(msg, "{\"enable_system\":false}");
			if(mqtt_publish(TOPIC_EN_OR_DIS_SYSTEM, msg))
			{
				gpio_set_level(LED_RED_PIN,   1);
				gpio_set_level(LED_GREEN_PIN, 0);
				ESP_LOGI(APP_MAIN_TAG, "System disabled!!");
				operation_status = true;
			}else
				operation_status = false;
			break;
		case OP_CONT_MODE:
			strcpy(msg, "{\"cam_mode\":1}");
			if(mqtt_publish(TOPIC_CAM_MODE, msg))
			{
				operation_status = true;
				ESP_LOGI(APP_MAIN_TAG, "Continuous mode enabled!!");
			}else
				operation_status = false;
			break;
		case OP_PERS_MODE:
			strcpy(msg, "{\"cam_mode\":2}");
			if(mqtt_publish(TOPIC_CAM_MODE, msg))
			{
				operation_status = true;
				ESP_LOGI(APP_MAIN_TAG, "Personalized mode enabled!!");
			}else
				operation_status = false;
			break;
		case OP_DIS_ALERT:
			strcpy(msg, "{\"disable\":true}");
			if(mqtt_publish(TOPIC_DIS_ALERT, msg))
			{
				operation_status = true;
				ESP_LOGI(APP_MAIN_TAG, "Alert disabled!!");
			}else
				operation_status = false;
			break;
		default:
			operation_status = false;
			ESP_LOGE(APP_MAIN_TAG, "Operation ID don't recognized: %d", action);
			break;
	}
	return operation_status;
}

