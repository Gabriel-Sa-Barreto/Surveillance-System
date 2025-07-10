cmake_minimum_required(VERSION 3.25.1)
project(Cam_Control)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")

find_package(PkgConfig REQUIRED)
find_package(OpenCV REQUIRED)
pkg_check_modules(LIBCAMERA REQUIRED libcamera)

include_directories(PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../libs)
include_directories(PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/include)
include_directories(PUBLIC /usr/local/include/libcamera)

add_library(log STATIC ${CMAKE_CURRENT_SOURCE_DIR}/../libs/src/log.c)
target_compile_definitions(log PUBLIC LOG_USE_COLOR)

add_library(gpio STATIC ${CMAKE_CURRENT_SOURCE_DIR}/../libs/src/gpio.c)
add_library(mqtt STATIC ${CMAKE_CURRENT_SOURCE_DIR}/../libs/src/MQTT.c)
add_library(manageCam STATIC ${CMAKE_CURRENT_SOURCE_DIR}/src/include/src/Manage_Camera.cpp)

add_executable(Cam_Control ${CMAKE_CURRENT_SOURCE_DIR}/src/app_main.cpp)

target_link_libraries(manageCam ${LIBCAMERA_LIBRARIES} ${OpenCV_LIBS})

# Link system's libraries
target_link_libraries(Cam_Control gpiod paho-mqtt3c cjson)

# Link intern libraries
target_link_libraries(Cam_Control log gpio mqtt manageCam)
