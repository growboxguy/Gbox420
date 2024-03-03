cmake_minimum_required(VERSION 3.12)

# Pull in SDK (must be before project)
include ($ENV{PICO_SDK_PATH}/pico_sdk_init.cmake)

project(FrequencyChanger C CXX ASM)
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

add_executable(FrequencyChanger
        FrequencyChanger.cpp
        )
target_compile_definitions(FrequencyChanger PRIVATE
        )
target_include_directories(FrequencyChanger PRIVATE 
        ${CMAKE_CURRENT_LIST_DIR}
)
target_link_libraries(FrequencyChanger
        pico_stdlib
        hardware_pwm
        )

pico_add_extra_outputs(FrequencyChanger)

pico_enable_stdio_usb(FrequencyChanger 1) # enable usb output
pico_enable_stdio_uart(FrequencyChanger 1) # enable uart output
