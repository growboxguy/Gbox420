cmake_minimum_required(VERSION 3.12)

# Pull in SDK (must be before project)
include ($ENV{PICO_SDK_PATH}/pico_sdk_init.cmake)

project(MqttTestMulti C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(PICO_BOARD "pico_w")


if (PICO_SDK_VERSION_STRING VERSION_LESS "1.3.0")
    message(FATAL_ERROR "Raspberry Pi Pico SDK version 1.3.0 (or later) required. Your version is ${PICO_SDK_VERSION_STRING}")
endif()

set(PICO_EXAMPLES_PATH ${PROJECT_SOURCE_DIR})

# Initialize the SDK
pico_sdk_init()

#enables almost all compiler warnings
add_compile_options(-Wall)    

add_executable(MqttTestMulti
        MqttTestMulti.cpp
        MqttClient.cpp       
        )

target_include_directories(MqttTestMulti PRIVATE 
        ${CMAKE_CURRENT_LIST_DIR}
)
target_link_libraries(MqttTestMulti
        pico_cyw43_arch_lwip_threadsafe_background
        pico_stdlib
        pico_lwip_mqtt
        )      

pico_add_extra_outputs(MqttTestMulti)

pico_enable_stdio_usb(MqttTestMulti 1) # enable usb output
pico_enable_stdio_uart(MqttTestMulti 1) # enable uart output

