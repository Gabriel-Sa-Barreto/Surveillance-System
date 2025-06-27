cmake_minimum_required(VERSION 3.25.1)
project(HM_Control)

include_directories(PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../libs)
include_directories(PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/include)

add_library(log STATIC ${CMAKE_CURRENT_SOURCE_DIR}/../libs/src/log.c)
target_compile_definitions(log PUBLIC LOG_USE_COLOR)

add_library(gpio STATIC ${CMAKE_CURRENT_SOURCE_DIR}/../libs/src/gpio.c)
add_library(mqtt STATIC ${CMAKE_CURRENT_SOURCE_DIR}/../libs/src/MQTT.c)
add_library(timer STATIC ${CMAKE_CURRENT_SOURCE_DIR}/../libs/src/Timer.c)

add_executable(HM_Control ${CMAKE_CURRENT_SOURCE_DIR}/src/app_main.c)

add_library(controller_led_bt STATIC ${CMAKE_CURRENT_SOURCE_DIR}/src/include/src/controller_led_bt.c)

# Link "controller_led_bt" libraries with its dependencies
target_link_libraries(controller_led_bt gpio timer)

# Link system's libraries
target_link_libraries(HM_Control gpiod paho-mqtt3c cjson)

# Link intern libraries
target_link_libraries(HM_Control log gpio mqtt timer controller_led_bt)
