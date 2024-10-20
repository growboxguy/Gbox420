add_library(pico_tone INTERFACE)

pico_generate_pio_header(pico_tone ${CMAKE_CURRENT_LIST_DIR}/pdm_pushout.pio)

target_sources(pico_tone INTERFACE
  ${CMAKE_CURRENT_LIST_DIR}/pico_tone.cpp
  ${CMAKE_CURRENT_LIST_DIR}/tone_dma_handlers.c
)

target_include_directories(pico_tone INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)

# Pull in pico libraries that we need
target_link_libraries(pico_tone INTERFACE pico_stdlib hardware_pio hardware_dma hardware_irq)