# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(tone_example C CXX ASM)

pico_sdk_init()

# Add executable. Default name is the project name, 
add_executable(tone_example tone_example.cpp)
add_subdirectory(../pico-tone build)

pico_add_extra_outputs(tone_example)
target_link_libraries(tone_example pico_stdlib pico_tone)
