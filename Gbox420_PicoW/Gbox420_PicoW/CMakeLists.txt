cmake_minimum_required(VERSION 3.12)

# Pull in SDK (must be before project)
include ($ENV{PICO_SDK_PATH}/pico_sdk_init.cmake)

project(Gbox420_PicoW C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(PICO_BOARD "pico_w")

if (PICO_SDK_VERSION_STRING VERSION_LESS "1.3.0")
    message(FATAL_ERROR "Raspberry Pi Pico SDK version 1.3.0 (or later) required. Your version is ${PICO_SDK_VERSION_STRING}")
endif()

set(PICO_EXAMPLES_PATH ${PROJECT_SOURCE_DIR})

# Initialize the SDK
pico_sdk_init()

add_compile_options(-Wall
        -Wno-format          # int != int32_t as far as the compiler is concerned because gcc has int32_t as long int
        -Wno-unused-function # we have some for the docs that aren't called
        -Wno-maybe-uninitialized
        )       

 file(GLOB_RECURSE SOURCES
        Components/*.h
        Components/*.cpp
    )  

add_executable(Gbox420_PicoW
        Gbox420_PicoW.cpp
        Settings.cpp
        Settings.h
        ${SOURCES}
        )
target_include_directories(Gbox420_PicoW PRIVATE 
        ${CMAKE_CURRENT_LIST_DIR}
)
target_link_libraries(Gbox420_PicoW
        pico_cyw43_arch_lwip_threadsafe_background
        pico_stdlib
        pico_lwip_mqtt
        hardware_rtc
        hardware_pwm
        )      

pico_add_extra_outputs(Gbox420_PicoW)

pico_enable_stdio_usb(Gbox420_PicoW 1) # enable usb output
pico_enable_stdio_uart(Gbox420_PicoW 1) # enable uart output

