cmake_minimum_required(VERSION 3.16)

add_library(freertos
    FreeRTOS-Kernel/list.c
    FreeRTOS-Kernel/queue.c
    FreeRTOS-Kernel/tasks.c
    FreeRTOS-Kernel/timers.c
    FreeRTOS-Kernel/event_groups.c
    FreeRTOS-Kernel/stream_buffer.c

    FreeRTOS-Kernel/portable/GCC/ARM_CM0/port.c
    FreeRTOS-Kernel/portable/GCC/ARM_CM0/portasm.c
    FreeRTOS-Kernel/portable/MemMang/heap_4.c
)

target_include_directories(freertos PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/FreeRTOS-Kernel/include
    ${CMAKE_CURRENT_SOURCE_DIR}/FreeRTOS-Kernel/portable/GCC/ARM_CM0
    ${PROJECT_SOURCE_DIR}/include            # Where your FreeRTOSConfig.h is
)

target_compile_options(freertos PRIVATE
    # Sets the correct processor architecture
    -mcpu=cortex-m0plus 
    
    # Ensures all code uses the Thumb instruction set (fixes ARM mode errors)
    -mthumb
    
    # Essential for Cortex-M0/M0+ FreeRTOS ports to fix assembly issues
    -mgeneral-regs-only
    
    # Ensures soft-float ABI is used, as M0+ has no FPU
    -mfloat-abi=soft 
)