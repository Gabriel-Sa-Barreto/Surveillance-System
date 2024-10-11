#include "headers/MQTT.h"

bool mqttStatus = false;
TaskHandle_t             mqttTaskHandle;
esp_mqtt_client_handle_t client;
alertData                alertReceived;  

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
        xTaskNotify(mqttTaskHandle, MQTT_CONNECTED, eSetValueWithOverwrite);
        break;
    case MQTT_EVENT_DISCONNECTED:
        mqttStatus = false;
        ESP_LOGI(MQTT_TAG, "MQTT disconnected.");
        xTaskNotify(mqttTaskHandle, RECONNECT, eSetValueWithOverwrite);
        break;
    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(MQTT_TAG, "Topic MQTT subscribed, msg_if=%d", event_data->msg_id);
        break;
    case MQTT_EVENT_UNSUBSCRIBED:
        break;
    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI(MQTT_TAG, "MQTT message published, msg_if=%d", event_data->msg_id);
        xTaskNotify(mqttTaskHandle, MQTT_PUBLISHED, eSetValueWithOverwrite);
        break;
    case MQTT_EVENT_DATA:
        ESP_LOGI(MQTT_TAG, "Message Received, msg_if=%d", event_data->msg_id);
        printf("Topic: %.*s\r\n", event_data->topic_len, event_data->topic);
        printf("Data: %.*s\r\n", event_data->data_len, event_data->data);
        if(strcmp(event_data->topic, TOPIC_ALERT) == 0)
        {
            {
                char *camera_id_pos = strstr(event_data->data, "\"camera_id\":");
                if (camera_id_pos != NULL) {
                    sscanf(camera_id_pos, "\"camera_id\":%d", &alertReceived.cameraID);
                }

                char *zone_pos = strstr(event_data->data, "\"zone\":\"");
                if (zone_pos != NULL) {
                    sscanf(zone_pos, "\"zone\":\"%[^\"]\"", alertReceived.zone);
                }
            }
            xTaskNotify(mqttTaskHandle, MQTT_ALERT_RECEIVED, eSetValueWithOverwrite);
        }
        break;
    case MQTT_EVENT_ERROR:
        ESP_LOGI(MQTT_TAG, "MQTT Error");
        break;
    default:
        ESP_LOGI(MQTT_TAG, "Mqtt Event unknown - id: %d", event_data->event_id);
        break;
    }
}

bool mqtt_connected()
{
    return mqttStatus;
}

TaskHandle_t* get_mqttHandle()
{
    return &mqttTaskHandle;
}

void mqtt_connection()
{
    while(mqttTaskHandle == NULL){ vTaskDelay(pdMS_TO_TICKS(100));}
    xTaskNotify(mqttTaskHandle, NETWORK_CONNECTED, eSetValueWithOverwrite);    	
}

void mqtt_publish(char *topic, char *msg)
{
    esp_mqtt_client_publish(client, topic, msg, strlen(msg), 2, false);
}

void task_MQTT_Controller(void *args)
{
    // Handler of the main task. Used to send notifications;
    mqttParameter *parameters = (mqttParameter *) args;
    uint32_t     command     = 0;
    esp_mqtt_client_config_t mqttConfig = {
        .broker.address.uri                  = MQTT_URI,
        .credentials.client_id               = MQTT_CLIENT_ID,
        .credentials.username                = MQTT_username,
        .credentials.authentication.password = MQTT_password
    };

    client = NULL;

    while(1)
    {
        xTaskNotifyWait(0,0,&command, portMAX_DELAY);
        switch (command)
        {
            case NETWORK_CONNECTED:
                // Wifi is connected. Now connected MQTT Broker
                client = esp_mqtt_client_init(&mqttConfig);
                esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_eventHandler, client);
                esp_mqtt_client_start(client);
                ESP_LOGI(MQTT_TAG, "MQTT is online");
                break;
            case MQTT_CONNECTED:
                //It's possible publish, subscribe in this state.
                esp_mqtt_client_subscribe(client, TOPIC_ALERT, 2);
                break;
            case MQTT_PUBLISHED:
                ESP_LOGI(MQTT_TAG, "MQTT publish notification sent to main task.");
                xTaskNotify(parameters->mainTaskHandler, MQTT_PUBLISHED, eSetValueWithOverwrite);
                break;
            case MQTT_ALERT_RECEIVED:
                ESP_LOGI(MQTT_TAG, "Alert received!!");
                xQueueSend(parameters->alertQueue, &alertReceived, pdTICKS_TO_MS(0));
                ESP_LOGI(MQTT_TAG, "Alert sent to main task!!");
                break;
            case RECONNECT:
                vTaskDelay(pdTICKS_TO_MS(1000));
                esp_mqtt_client_reconnect(client);
                break;
            default:
                ESP_LOGI(MQTT_TAG, "Any event recognised.");
                break;
        }

    }
}