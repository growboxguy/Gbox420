cmake_minimum_required(VERSION 3.13)

# Pull in SDK (must be before project)
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)
# Pull in FreeRTOS
include(FreeRTOS_Kernel_import.cmake)

project(Gbox420_PicoW_FreeRTOS C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(PICO_BOARD "pico_w")

# Initialize the SDK
pico_sdk_init()

#enables almost all compiler warnings
add_compile_options(-Wall)    

# Collect all .cpp and .h files in the specified directory
file(GLOB SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
file(GLOB COMPONENT_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/Components/*.cpp")
file(GLOB COMPONENT_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/Components/*.h")

add_executable(Gbox420_PicoW_FreeRTOS Gbox420_PicoW_FreeRTOS.cpp ${SOURCES} ${HEADERS} ${COMPONENT_SOURCES} ${COMPONENT_HEADERS})

target_compile_definitions(Gbox420_PicoW_FreeRTOS PRIVATE)

target_include_directories(Gbox420_PicoW_FreeRTOS PRIVATE ${CMAKE_CURRENT_LIST_DIR})
 

target_link_libraries(Gbox420_PicoW_FreeRTOS
    pico_stdlib
    pico_cyw43_arch_lwip_sys_freertos
    FreeRTOS-Kernel
    FreeRTOS-Kernel-Heap4
    hardware_rtc
    )

# create map/bin/hex/uf2 file etc.
pico_add_extra_outputs(Gbox420_PicoW_FreeRTOS)

# enable usb output, enable uart output
pico_enable_stdio_usb(Gbox420_PicoW_FreeRTOS 1)
pico_enable_stdio_uart(Gbox420_PicoW_FreeRTOS 1)

