project(private_functions)
add_library(${PROJECT_NAME})
target_sources(${PROJECT_NAME}
    PRIVATE
        private_functions.cpp
        private_functions.h        
)

target_include_directories(${PROJECT_NAME}  PUBLIC 
    ../../inc
    )
    
target_link_libraries(${PROJECT_NAME} 
    PRIVATE 
        // cmsis_core
        pico_stdlib
        hardware_gpio
        hardware_pwm
        hardware_spi
        
        // pico_multicore
        // hardware_sync
        hardware_pio
        hardware_dma
        hardware_irq

)