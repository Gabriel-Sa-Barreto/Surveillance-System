#include "connect.h"

/* The event group allows multiple bits for each event, but we only care about two events:
 * - we are connected to the AP with an IP
 * - we failed to connect after the maximum amount of retries */
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1

/* FreeRTOS event group to signal when Wi-fi is connected*/
static EventGroupHandle_t s_wifi_event_group;

/* Number of times to try reconnecting to wifi when the DISCONNECTED event is detected*/
static int s_retry_num     = 0;
static int S_MAXIMUM_RETRY = 5;

bool wifiConnected = false;

char *get_wifi_err(uint8_t reason)
{
    switch (reason)
    {
        case WIFI_REASON_UNSPECIFIED:
            return "WIFI_REASON_UNSPECIFIED";
        case WIFI_REASON_AUTH_EXPIRE:
            return "WIFI_REASON_AUTH_EXPIRE";
        case WIFI_REASON_AUTH_LEAVE:
            return "WIFI_REASON_AUTH_LEAVE";
        case WIFI_REASON_ASSOC_EXPIRE:
            return "WIFI_REASON_ASSOC_EXPIRE";
        case WIFI_REASON_ASSOC_TOOMANY:
            return "WIFI_REASON_ASSOC_TOOMANY";
        case WIFI_REASON_NOT_AUTHED:
            return "WIFI_REASON_NOT_AUTHED";
        case WIFI_REASON_NOT_ASSOCED:
            return "WIFI_REASON_NOT_ASSOCED";
        case WIFI_REASON_ASSOC_LEAVE:
            return "WIFI_REASON_ASSOC_LEAVE";
        case WIFI_REASON_ASSOC_NOT_AUTHED:
            return "WIFI_REASON_ASSOC_NOT_AUTHED";
        case WIFI_REASON_DISASSOC_PWRCAP_BAD:
            return "WIFI_REASON_DISASSOC_PWRCAP_BAD";
        case WIFI_REASON_DISASSOC_SUPCHAN_BAD:
            return "WIFI_REASON_DISASSOC_SUPCHAN_BAD";
        case WIFI_REASON_BSS_TRANSITION_DISASSOC:
            return "WIFI_REASON_BSS_TRANSITION_DISASSOC";
        case WIFI_REASON_IE_INVALID:
            return "WIFI_REASON_IE_INVALID";
        case WIFI_REASON_MIC_FAILURE:
            return "WIFI_REASON_MIC_FAILURE";
        case WIFI_REASON_4WAY_HANDSHAKE_TIMEOUT:
            return "WIFI_REASON_4WAY_HANDSHAKE_TIMEOUT";
        case WIFI_REASON_GROUP_KEY_UPDATE_TIMEOUT:
            return "WIFI_REASON_GROUP_KEY_UPDATE_TIMEOUT";
        case WIFI_REASON_IE_IN_4WAY_DIFFERS:
            return "WIFI_REASON_IE_IN_4WAY_DIFFERS";
        case WIFI_REASON_GROUP_CIPHER_INVALID:
            return "WIFI_REASON_GROUP_CIPHER_INVALID";
        case WIFI_REASON_PAIRWISE_CIPHER_INVALID:
            return "WIFI_REASON_PAIRWISE_CIPHER_INVALID";
        case WIFI_REASON_AKMP_INVALID:
            return "WIFI_REASON_AKMP_INVALID";
        case WIFI_REASON_UNSUPP_RSN_IE_VERSION:
            return "WIFI_REASON_UNSUPP_RSN_IE_VERSION";
        case WIFI_REASON_INVALID_RSN_IE_CAP:
            return "WIFI_REASON_INVALID_RSN_IE_CAP";
        case WIFI_REASON_802_1X_AUTH_FAILED:
            return "WIFI_REASON_802_1X_AUTH_FAILED";
        case WIFI_REASON_CIPHER_SUITE_REJECTED:
            return "WIFI_REASON_CIPHER_SUITE_REJECTED";
        case WIFI_REASON_TDLS_PEER_UNREACHABLE:
            return "WIFI_REASON_TDLS_PEER_UNREACHABLE";
        case WIFI_REASON_TDLS_UNSPECIFIED:
            return "WIFI_REASON_TDLS_UNSPECIFIED";
        case WIFI_REASON_SSP_REQUESTED_DISASSOC:
            return "WIFI_REASON_SSP_REQUESTED_DISASSOC";
        case WIFI_REASON_NO_SSP_ROAMING_AGREEMENT:
            return "WIFI_REASON_NO_SSP_ROAMING_AGREEMENT";
        case WIFI_REASON_BAD_CIPHER_OR_AKM:
            return "WIFI_REASON_BAD_CIPHER_OR_AKM";
        case WIFI_REASON_NOT_AUTHORIZED_THIS_LOCATION:
            return "WIFI_REASON_NOT_AUTHORIZED_THIS_LOCATION";
        case WIFI_REASON_SERVICE_CHANGE_PERCLUDES_TS:
            return "WIFI_REASON_SERVICE_CHANGE_PERCLUDES_TS";
        case WIFI_REASON_UNSPECIFIED_QOS:
            return "WIFI_REASON_UNSPECIFIED_QOS";
        case WIFI_REASON_NOT_ENOUGH_BANDWIDTH:
            return "WIFI_REASON_NOT_ENOUGH_BANDWIDTH";
        case WIFI_REASON_MISSING_ACKS:
            return "WIFI_REASON_MISSING_ACKS";
        case WIFI_REASON_EXCEEDED_TXOP:
            return "WIFI_REASON_EXCEEDED_TXOP";
        case WIFI_REASON_STA_LEAVING:
            return "WIFI_REASON_STA_LEAVING";
        case WIFI_REASON_END_BA:
            return "WIFI_REASON_END_BA";
        case WIFI_REASON_UNKNOWN_BA:
            return "WIFI_REASON_UNKNOWN_BA";
        case WIFI_REASON_TIMEOUT:
            return "WIFI_REASON_TIMEOUT";
        case WIFI_REASON_PEER_INITIATED:
            return "WIFI_REASON_PEER_INITIATED";
        case WIFI_REASON_AP_INITIATED:
            return "WIFI_REASON_AP_INITIATED";
        case WIFI_REASON_INVALID_FT_ACTION_FRAME_COUNT:
            return "WIFI_REASON_INVALID_FT_ACTION_FRAME_COUNT";
        case WIFI_REASON_INVALID_PMKID:
            return "WIFI_REASON_INVALID_PMKID";
        case WIFI_REASON_INVALID_MDE:
            return "WIFI_REASON_INVALID_MDE";
        case WIFI_REASON_INVALID_FTE:
            return "WIFI_REASON_INVALID_FTE";
        case WIFI_REASON_TRANSMISSION_LINK_ESTABLISH_FAILED:
            return "WIFI_REASON_TRANSMISSION_LINK_ESTABLISH_FAILED";
        case WIFI_REASON_ALTERATIVE_CHANNEL_OCCUPIED:
            return "WIFI_REASON_ALTERATIVE_CHANNEL_OCCUPIED";
        case WIFI_REASON_BEACON_TIMEOUT:
            return "WIFI_REASON_BEACON_TIMEOUT";
        case WIFI_REASON_NO_AP_FOUND:
            return "WIFI_REASON_NO_AP_FOUND";
        case WIFI_REASON_AUTH_FAIL:
            return "WIFI_REASON_AUTH_FAIL";
        case WIFI_REASON_ASSOC_FAIL:
            return "WIFI_REASON_ASSOC_FAIL";
        case WIFI_REASON_HANDSHAKE_TIMEOUT:
            return "WIFI_REASON_HANDSHAKE_TIMEOUT";
        case WIFI_REASON_CONNECTION_FAIL:
            return "WIFI_REASON_CONNECTION_FAIL";
        case WIFI_REASON_AP_TSF_RESET:
            return "WIFI_REASON_AP_TSF_RESET";
        case WIFI_REASON_ROAMING:
            return "WIFI_REASON_ROAMING";
        case WIFI_REASON_ASSOC_COMEBACK_TIME_TOO_LONG:
            return "WIFI_REASON_ASSOC_COMEBACK_TIME_TOO_LONG";
        case WIFI_REASON_SA_QUERY_TIMEOUT:
            return "WIFI_REASON_SA_QUERY_TIMEOUT";
        case WIFI_REASON_NO_AP_FOUND_W_COMPATIBLE_SECURITY:
            return "WIFI_REASON_NO_AP_FOUND_W_COMPATIBLE_SECURITY";
        case WIFI_REASON_NO_AP_FOUND_IN_AUTHMODE_THRESHOLD:
            return "WIFI_REASON_NO_AP_FOUND_IN_AUTHMODE_THRESHOLD";
        case WIFI_REASON_NO_AP_FOUND_IN_RSSI_THRESHOLD:
            return "WIFI_REASON_NO_AP_FOUND_IN_RSSI_THRESHOLD";
    }
    return "WIFI_REASON_UNSPECIFIED";
}

static void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    switch (event_id)
    {
    case WIFI_EVENT_STA_START:
        ESP_LOGI(WIFI_TAG,"wifi event started.");
        wifiConnected = false;
        esp_wifi_connect();
        break;
    case WIFI_EVENT_STA_DISCONNECTED:
        {
            wifiConnected = false;
            wifi_event_sta_disconnected_t *wifi_event_sta_disconnected = event_data;
            ESP_LOGE(WIFI_TAG, "wifi error: %s", get_wifi_err(wifi_event_sta_disconnected->reason));
            if (s_retry_num < S_MAXIMUM_RETRY)
            {
                esp_wifi_connect();
                s_retry_num++;
                ESP_LOGI(WIFI_TAG, "retry to connect to the AP....");
            } else
            {
                s_retry_num = 0;
                xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
            }
            ESP_LOGI(WIFI_TAG,"connect to the AP fail.");
            break;
        }
    case IP_EVENT_STA_GOT_IP:
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(WIFI_TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num   = 0;
        wifiConnected = true;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
        break;
    default:
        break;
    }
}

esp_err_t wifi_disconnect(void)
{
    esp_err_t wifi_disc = esp_wifi_disconnect();
    esp_wifi_stop();
    return wifi_disc;
}

bool wifi_isConnected(void)
{
    return wifiConnected;
}

void wifi_connect(void)
{
    ESP_LOGI(WIFI_TAG, "Trying connection....");
    esp_wifi_connect();
}

esp_err_t initWifi()
{
    s_wifi_event_group = xEventGroupCreate();

    ESP_ERROR_CHECK(esp_netif_init());

    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        &instance_got_ip));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PWD
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config) );
    ESP_ERROR_CHECK(esp_wifi_start() );

    ESP_LOGI(WIFI_TAG, "wifi_init_sta finished.");

    EventBits_t bits = WIFI_FAIL_BIT;
    while(bits & WIFI_FAIL_BIT)
    {
        /* Waiting until either the connection is established (WIFI_CONNECTED_BIT) or connection failed for the maximum
        * number of re-tries (WIFI_FAIL_BIT). The bits are set by event_handler() (see above) */
        bits = xEventGroupWaitBits(s_wifi_event_group,
                WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
                pdTRUE,
                pdFALSE,
                portMAX_DELAY);

        /* xEventGroupWaitBits() returns the bits before the call returned, hence we can test which event actually
        * happened. */
        if (bits & WIFI_FAIL_BIT)
        {
            ESP_LOGE(WIFI_TAG, "failed to connect to SSID:%s", WIFI_SSID);
            wifi_connect();
        }else
        {
            ESP_LOGI(WIFI_TAG, "connected to ap SSID:%s", WIFI_SSID);
        }
    }
    return ESP_OK;
}