cmake_minimum_required(VERSION 3.16)

add_library(freertos
    FreeRTOS-Kernel/list.c
    FreeRTOS-Kernel/queue.c
    FreeRTOS-Kernel/tasks.c
    FreeRTOS-Kernel/timers.c
    FreeRTOS-Kernel/event_groups.c
    FreeRTOS-Kernel/stream_buffer.c

    portable/GCC/ARM_CM0/port.c
    portable/MemMang/heap4.c
)

target_include_directories(freertos PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/FreeRTOS-Kernel/include
    ${CMAKE_CURRENT_SOURCE_DIR}/portable/GCC/ARM_CM0
    ${PROJECT_SOURCE_DIR}/include            # Where your FreeRTOSConfig.h is
)
