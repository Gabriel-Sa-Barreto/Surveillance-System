cmake_minimum_required(VERSION 3.25.1)
project(Get_Cam_Img)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")

find_package(PkgConfig REQUIRED)
find_package(OpenCV REQUIRED)
pkg_check_modules(LIBCAMERA REQUIRED libcamera)

include_directories(PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../libs)
include_directories(PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/include)

add_library(log STATIC ${CMAKE_CURRENT_SOURCE_DIR}/../libs/src/log.c)
add_library(gpio STATIC ${CMAKE_CURRENT_SOURCE_DIR}/../libs/src/gpio.c)
add_library(mqtt STATIC ${CMAKE_CURRENT_SOURCE_DIR}/../libs/src/MQTT.c)
target_compile_definitions(log PUBLIC LOG_USE_COLOR)

add_executable(Get_Cam_Img ${CMAKE_CURRENT_SOURCE_DIR}/src/app_main.cpp)

# Link system's libraries
target_link_libraries(Get_Cam_Img gpiod paho-mqtt3c cjson ${OpenCV_LIBS})

# Link intern libraries
target_link_libraries(Get_Cam_Img log gpio mqtt)
