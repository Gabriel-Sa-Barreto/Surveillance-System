#include "../MQTT.h"

MQTTClient client;
MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;

static char* mqtt_error_code(int code)
{
    switch(code)
    {
        case MQTTCLIENT_SUCCESS:               return "MQTTCLIENT_SUCCESS";               break;
        case MQTTCLIENT_FAILURE:               return "MQTTCLIENT_FAILURE";               break;
        case MQTTCLIENT_DISCONNECTED:          return "MQTTCLIENT_DISCONNECTED";          break;
        case MQTTCLIENT_MAX_MESSAGES_INFLIGHT: return "MQTTCLIENT_MAX_MESSAGES_INFLIGHT"; break;
        case MQTTCLIENT_BAD_UTF8_STRING:       return "MQTTCLIENT_BAD_UTF8_STRING";       break;
        case MQTTCLIENT_BAD_STRUCTURE:         return "MQTTCLIENT_BAD_STRUCTURE";         break;
        case MQTTCLIENT_BAD_MQTT_VERSION:      return "MQTTCLIENT_BAD_MQTT_VERSION";      break;
        case MQTTCLIENT_BAD_PROTOCOL:          return "MQTTCLIENT_BAD_PROTOCOL";          break;
        case MQTTCLIENT_BAD_MQTT_OPTION:       return "MQTTCLIENT_BAD_MQTT_OPTION";       break;
        case MQTTCLIENT_WRONG_MQTT_VERSION:    return "MQTTCLIENT_WRONG_MQTT_VERSION";    break;
        default: return "Unknown error"; break;
    }
}

static void on_disconnected(void *context, char *cause)
{
    log_error("Connection with server was lost...");
    log_info("Trying reconnection...");
    while(MQTTClient_connect(client, &conn_opts) != MQTTCLIENT_SUCCESS)
    {
        log_info("Waiting %ds....", MQTT_RECONNECTION_TIME);
        sleep(MQTT_RECONNECTION_TIME);
    }
    log_info("MQTT connected successfully.");
}

int mqtt_init(int (*onMessage)(void *context, char *topicName, int topicLen, MQTTClient_message *message))
{
    int rc = MQTTClient_create(&client, MQTT_ADDRESS, MQTT_CLIENT_ID, MQTTCLIENT_PERSISTENCE_NONE, NULL);

    if(MQTTClient_setCallbacks(client, NULL, on_disconnected, onMessage, NULL) != MQTTCLIENT_SUCCESS)
    {
        log_error("Error to set mqtt function callback.");	
        return MQTTCLIENT_FAILURE;
    }
    if(rc != MQTTCLIENT_SUCCESS)
    {
        log_error("Error to create mqtt client.");
        return MQTTCLIENT_FAILURE;
    }
    else
        log_info("MQTT Client created successfully");

    conn_opts.keepAliveInterval = 60;
    conn_opts.cleansession      = 1;
    conn_opts.username          = MQTT_USERNAME;
    conn_opts.password          = MQTT_PASSWORD;

    rc = MQTTClient_connect(client, &conn_opts);

    if(rc != MQTTCLIENT_SUCCESS)
        log_error("Error to connect MQTT: %s (%d)", mqtt_error_code(rc), rc);
    else
        log_info("MQTT connected successfully.");

    return rc;
}

int mqtt_subscribe(char *topic)
{
    int rc = MQTTClient_subscribe(client, topic, MQTT_QOS);
    if(rc != MQTTCLIENT_SUCCESS)
    {
        log_error("Error to subscribe topic on mqtt: %s (%d)", mqtt_error_code(rc), rc);
        return MQTTCLIENT_FAILURE;
    }else
        log_info("MQTT topic subscribed successfully: %s", topic);
    return MQTTCLIENT_SUCCESS;
}

int mqtt_publish(char *topic, char *payload, int retained)
{
    int rc = MQTTClient_publish(client, topic, strlen(payload), (void*)payload, MQTT_QOS, retained, NULL);	
    if(rc != MQTTCLIENT_SUCCESS)
    {
        log_error("Error to publish message on mqtt topic : %s. Error: %s (%d)", topic, mqtt_error_code(rc), rc);
        return MQTTCLIENT_FAILURE;
    }else
        log_info("Message published on topic (%s) successfully.", topic);
    return MQTTCLIENT_SUCCESS;
}