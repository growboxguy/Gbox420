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

add_executable(Gbox420_PicoW
        Gbox420_PicoW.c
        )
target_include_directories(Gbox420_PicoW PRIVATE 
        ${CMAKE_CURRENT_LIST_DIR}
        $ENV{PICO_SDK_PATH}/../pico-examples/pico_w/wifi  #include "lwipopts_examples_common.h"
)
target_link_libraries(Gbox420_PicoW
        pico_cyw43_arch_lwip_threadsafe_background
        pico_stdlib
        hardware_rtc
        )

pico_add_extra_outputs(Gbox420_PicoW)

# enable usb output, disable uart output
pico_enable_stdio_usb(Gbox420_PicoW 1)
pico_enable_stdio_uart(Gbox420_PicoW 0)

