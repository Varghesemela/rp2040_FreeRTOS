project(ws2812)
file(GLOB FILES *.c *.h)
add_library(${PROJECT_NAME} ws2812.cpp ws2812.hpp)

pico_generate_pio_header(${PROJECT_NAME} ${CMAKE_CURRENT_LIST_DIR}/../../pio/ws2812.pio)

target_sources(${PROJECT_NAME}
    PRIVATE
        ws2812.cpp
        ws2812.hpp     
)

target_include_directories(${PROJECT_NAME}
    PUBLIC

)

target_link_libraries(${PROJECT_NAME}
    PUBLIC 
        cmsis_core
        pico_stdlib
        pico_multicore
        hardware_pio
        hardware_dma
        hardware_irq
        hardware_uart
        hardware_adc
        hardware_spi
        hardware_i2c
        hardware_flash
        hardware_sync
        hardware_watchdog
        hardware_pwm
        
        
)