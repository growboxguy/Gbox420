cmake_minimum_required(VERSION 3.13)

# Pull in SDK (must be before project)
include ($ENV{PICO_SDK_PATH}/pico_sdk_init.cmake)
# Pull in FreeRTOS
include(FreeRTOS_Kernel_import.cmake)

project(RealTimeClock-FreeRTOS C CXX ASM)
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

add_executable(RealTimeClock-FreeRTOS
        RealTimeClock-FreeRTOS.cpp
        )

target_compile_definitions(RealTimeClock-FreeRTOS PRIVATE
        )
target_include_directories(RealTimeClock-FreeRTOS PRIVATE 
        ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(RealTimeClock-FreeRTOS
        pico_stdlib
        pico_cyw43_arch_lwip_sys_freertos
        FreeRTOS-Kernel
        FreeRTOS-Kernel-Heap4
        hardware_rtc
        pico_lwip_sntp
        )

pico_add_extra_outputs(RealTimeClock-FreeRTOS)

pico_enable_stdio_usb(RealTimeClock-FreeRTOS 1) # enable usb output
pico_enable_stdio_uart(RealTimeClock-FreeRTOS 1) # enable uart output

