#include "headers/packageProcessor.h"

QueueHandle_t queueHandler_pkgReceived;
QueueHandle_t queueHandler_alerts;
MQTT_package  pkg_data;

QueueHandle_t* get_queueHandler_alert()
{
    return &queueHandler_alerts;
}

void task_packageProcessor(void *args)
{
    char zone[ZONE_NAME_SIZE];
    //Gets the handler of output queue of MQTT.
    QueueHandle_t *handler   = (QueueHandle_t *) args;
    queueHandler_pkgReceived = *handler; 

    queueHandler_alerts      = xQueueCreate(1, sizeof(zone));

    while(xQueueReceive(queueHandler_pkgReceived, &pkg_data, portMAX_DELAY))
    {
        if(strcmp(pkg_data.topic, TOPIC_ALERT) == 0)
        {
            /* ALERT PROCESSING */
            char *aux;
            aux = strstr(pkg_data.data, "\"zone\":");
            if(aux != NULL)
            {
                sscanf(aux, "\"zone\":\"%[^\"]", zone);
                ESP_LOGI(PKG_PROCESSOR_TAG, "RECEIVED ALERT: %s", zone);
                xQueueSend(queueHandler_alerts, zone, portMAX_DELAY);
            }
        }
        else if(strcmp(pkg_data.topic, TOPIC_RECEIVED_PWD) == 0)
        {
            /* REFRESHING SYSTEM PASSWORD */
            char *aux;
            char  pwd[PASSWORD_SIZE + 1];
            aux = strstr(pkg_data.data, "\"new_pwd\":");
            if(aux != NULL)
            {
                sscanf(aux, "\"new_pwd\":\"%[^\"]", pwd);
                ESP_LOGI(PKG_PROCESSOR_TAG, "New pwd received: %s", pwd);
                // Storage the new password
                FILE *ptr_file = openFile(PWD_PATH, "w");
                fprintf(ptr_file, pwd);
                fclose(ptr_file);
                ESP_LOGI(PKG_PROCESSOR_TAG, "Password refreshed.");
            }
        }
    }
}
