#ifndef __GENERAL_H
    #define __GENERAL_H
    /* Wifi Credentials */
    #define WIFI_SSID "LSNET_BARRETO"
    #define WIFI_PWD "Tuty7090"

    /* MQTT Credentials */
    #define MQTT_URI        "mqtt://test.mosquitto.org:1883" //"mqtt://broker.hivemq.com:1883"
    #define MQTT_CLIENT_ID  "kshdfsfd27/09/2024-15:40"
    #define MQTT_username   ""
    #define MQTT_password   ""  

    /* Communicate topic between the cameras and the control panel */
    #define TOPIC_EN_OR_DIS_SYSTEM     "/sys/cam/activated/"
    #define TOPIC_ALERT                "/sys/cam/alert/"
    #define TOPIC_CAM_MODE             "/sys/cam/mode/"
    #define TOPIC_RECEIVED_PWD         "/sys/pwd/"
    #define TOPIC_DIS_ALERT            "/sys/cam/alert/dis/"

    #define PWD_PATH "/littlefs/password.txt"
    #define PASSWORD_SIZE 5

    /* Hardware Mapping */
    #define LED_RED_PIN   5
    #define LED_GREEN_PIN 19
    #define BT_1_PIN      39
    #define BT_2_PIN      34
    #define BT_3_PIN      35
    #define M_KEY_LINE_0_PIN    4
    #define M_KEY_LINE_1_PIN    16
    #define M_KEY_LINE_2_PIN    17
    #define M_KEY_LINE_3_PIN    18
    #define M_KEY_COL_0_PIN     27
    #define M_KEY_COL_1_PIN     26
    #define M_KEY_COL_2_PIN     25
    #define M_KEY_NUMBER_COL    3
    #define M_KEY_NUMBER_LINE   4
    /* Tags to tasks */
    #define WIFI_TAG       "WIFI"
    #define MQTT_TAG       "MQTT"
    #define DISPLAY_TAG    "DISPLAY_OLED"
    #define BUTTON_TAG     "BUTTON"
    #define M_KEY_TAG      "M_KEYBOARD" 
    #define CONTROLLER_TAG "CONTROLLER"
    #define APP_MAIN_TAG   "APP_MAIN"
    #define TIMER_TAG      "TIMERS"
    #define PKG_PROCESSOR_TAG "PKG_PROCESSOR"
    #define FILE_SYSTEM_TAG "LITTLE_FS"
    #define SNTP_TAG "SNTP"
    /* OLED Display dimensions */
    #define DISPLAY_LINE 64
    #define DISPLAY_COL  128

    #define BITMAP_SCREEN_CURSOR_LINE 8
    #define BITMAP_SCREEN_CURSOR_COL  16
    #define BITMAP_CONFIRMATION_LINE 32
    #define BITMAP_CONFIRMATION_COL 40
    #define BITMAP_ERROR_LINE 32
    #define BITMAP_ERROR_COL 32
    #define D_FONT_SIZE_LINE 8
    #define D_FONT_SIZE_COL  8

    /* TASK PRIORITY */
    #define PR_MAIN_TASK 2
    #define PR_DISPLAY_TASK 1
    #define PR_MQTT_TASK 3
    #define PR_PKG_PROCESSOR_TASK 2

    /* SIZE OF RECEIVED PACKAGES */
    #define PKG_SIZE 60
    #define ZONE_NAME_SIZE 20

    /* SNTP */
    #define SNTP_ENABLE 1

    enum buttonsID
    {
        BUTTON_UP = 1,
        BUTTON_DOWN,
        BUTTON_ENTER
    };

    enum displayOled_pages
    {
        D_PAGE_0,
        D_PAGE_1,
        D_PAGE_2,
        D_PAGE_3,
        D_PAGE_4,
        D_PAGE_5,
        D_PAGE_6,
        D_PAGE_7
    };
    /* Struct that stores the received data from MQTT */
    typedef struct
    {
        char topic[25];
        char data[PKG_SIZE];
    } MQTT_package;

#endif