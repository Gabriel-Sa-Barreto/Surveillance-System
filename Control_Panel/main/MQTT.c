#include "headers/MQTT.h"

bool mqttStatus = false;

MQTT_package mqttPackage;

/* MQTT Handles */
TaskHandle_t             taskHandler_mqtt;
QueueHandle_t            queueHandler_packages;
esp_mqtt_client_handle_t client;

static void mqtt_eventHandler(void* event_handler_arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    // State Machine to process the events
    mqtt_event_handler_callback(event_data);
}

void mqtt_event_handler_callback(esp_mqtt_event_handle_t event_data)
{
    switch (event_data->event_id)
    {
        case MQTT_EVENT_CONNECTED:
            mqttStatus = true;
            ESP_LOGI(MQTT_TAG, "MQTT is connected.");
            xTaskNotify(taskHandler_mqtt, MQTT_CONNECTED, eSetValueWithOverwrite);
            break;
        case MQTT_EVENT_DISCONNECTED:
            mqttStatus = false;
            ESP_LOGI(MQTT_TAG, "MQTT disconnected.");
            xTaskNotify(taskHandler_mqtt, RECONNECT, eSetValueWithOverwrite);
            break;
        case MQTT_EVENT_SUBSCRIBED:
            ESP_LOGI(MQTT_TAG, "Topic MQTT subscribed, msg_if= %d", event_data->msg_id);
            break;
        case MQTT_EVENT_UNSUBSCRIBED:
            break;
        case MQTT_EVENT_PUBLISHED:
            ESP_LOGI(MQTT_TAG, "MQTT message published, msg_if= %d", event_data->msg_id);
            xTaskNotify(taskHandler_mqtt, MQTT_PUBLISHED, eSetValueWithOverwrite);
            break;
        case MQTT_EVENT_DATA:
            ESP_LOGI(MQTT_TAG, "Message Received, msg_if= %d", event_data->msg_id);
            ESP_LOGI(MQTT_TAG, "Topic: %.*s\r\n", event_data->topic_len, event_data->topic);
            ESP_LOGI(MQTT_TAG, "Data: %.*s\r\n", event_data->data_len, event_data->data);
            strcpy(mqttPackage.topic, event_data->topic);
            strcpy(mqttPackage.data, event_data->data);
            xTaskNotify(taskHandler_mqtt, MQTT_DATA_RECEIVED, eSetValueWithOverwrite);
            break;
        case MQTT_EVENT_ERROR:
            break;
        default:
            break;
    }
}

uint8_t mqtt_publish(char *topic, char *msg)
{
    return (uint8_t) esp_mqtt_client_publish(client, topic, msg, strlen(msg), 2, false);
}

bool mqtt_connected()
{
    return mqttStatus;
}

TaskHandle_t* get_mqttHandle()
{
    return &taskHandler_mqtt;
}

QueueHandle_t* getHandler_MQTTpackageQueue()
{
    return &queueHandler_packages;
}

void mqtt_connection()
{
    while(taskHandler_mqtt == NULL){ vTaskDelay(pdMS_TO_TICKS(100));}
    xTaskNotify(taskHandler_mqtt, NETWORK_CONNECTED, eSetValueWithOverwrite);    	
}

void task_MQTT_Controller(void *args)
{
    esp_mqtt_client_config_t mqttConfig = {
        .broker.address.uri                  = MQTT_URI,
        .credentials.client_id               = MQTT_CLIENT_ID,
        .credentials.username                = MQTT_username,
        .credentials.authentication.password = MQTT_password,
        .task.priority                       = PR_MQTT_TASK
    };

    queueHandler_packages = xQueueCreate(5, sizeof(MQTT_package));
    client            = NULL;
    uint32_t command  = 0;
    while (1)
    {
        xTaskNotifyWait(0,0,&command, portMAX_DELAY);
        switch (command)
        {
            case NETWORK_CONNECTED:
            // Wifi is connected. Now connected MQTT Broker
                client = esp_mqtt_client_init(&mqttConfig);
                ESP_ERROR_CHECK(esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_eventHandler, &client));
                ESP_ERROR_CHECK(esp_mqtt_client_start(client));
                ESP_LOGI(MQTT_TAG, "MQTT is initialized.");
                break;
            case MQTT_CONNECTED:
                esp_mqtt_client_subscribe(client, TOPIC_ALERT, 2);
                esp_mqtt_client_subscribe(client, TOPIC_RECEIVED_PWD, 2);
                break;
            case MQTT_PUBLISHED:
                break;
            case MQTT_DATA_RECEIVED:
                xQueueSend(queueHandler_packages, &mqttPackage, pdMS_TO_TICKS(200));
                break;
            case RECONNECT:
                vTaskDelay(pdTICKS_TO_MS(1000));
                esp_mqtt_client_reconnect(client);
                break;
            default:
                ESP_LOGE(MQTT_TAG, "Any event recognised.");
                break;
        }
    }
    
}