cmake_minimum_required(VERSION 3.20)

project("ucDSP" C CXX ASM)

include(cmake/st-project.cmake)

# Add FreeRTOS source files
set(FREERTOS_SOURCES
    freertos/source/portable/MemMang/heap_4.c
    freertos/source/portable/GCC/ARM_CM4F/port.c
    freertos/source/event_groups.c
    freertos/source/list.c
    freertos/source/queue.c
    freertos/source/tasks.c
    freertos/source/timers.c
)

# Add FreeRTOS include directories
include_directories(
    freertos/source/include
    freertos/source/portable/GCC/ARM_CM4F
    /Inc 
)



add_executable(${PROJECT_NAME})

# Add FreeRTOS sources to the target
target_sources(${PROJECT_NAME} PRIVATE ${FREERTOS_SOURCES})

add_st_target_properties(${PROJECT_NAME})
