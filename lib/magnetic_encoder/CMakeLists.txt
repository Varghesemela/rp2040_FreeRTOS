project(magnetic_encoder)

add_library(${PROJECT_NAME} encoder.cpp encoder.h)

target_sources(${PROJECT_NAME} PRIVATE
encoder.cpp
encoder.h)

target_link_libraries(${PROJECT_NAME}   PUBLIC
pico_stdlib
hardware_spi
hardware_dma
)
