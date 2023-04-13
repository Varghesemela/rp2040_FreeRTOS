project(freeRTOS)
set(PICO_SDK_FREERTOS_SOURCE FreeRTOS-Kernel)

add_library(${PROJECT_NAME}
    ${PICO_SDK_FREERTOS_SOURCE}/event_groups.c
    ${PICO_SDK_FREERTOS_SOURCE}/list.c
    ${PICO_SDK_FREERTOS_SOURCE}/queue.c
    ${PICO_SDK_FREERTOS_SOURCE}/stream_buffer.c
    ${PICO_SDK_FREERTOS_SOURCE}/tasks.c
    ${PICO_SDK_FREERTOS_SOURCE}/timers.c
    ${PICO_SDK_FREERTOS_SOURCE}/portable/MemMang/heap_3.c
    ${PICO_SDK_FREERTOS_SOURCE}/portable/ThirdParty/GCC/RP2040/port.c
)

target_include_directories(${PROJECT_NAME} PUBLIC
    .
    ${PICO_SDK_FREERTOS_SOURCE}/include
    ${PICO_SDK_FREERTOS_SOURCE}/portable/ThirdParty/GCC/RP2040/include
)

target_link_libraries(${PROJECT_NAME} PUBLIC
     pico_stdlib
     pico_multicore
     hardware_sync
     hardware_irq
     hardware_exception
    )