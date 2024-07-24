# Define the FreeRTOS source files
file(GLOB FREERTOS_SRC
    "portable/GCC/ARM_CM4F/port.c"
    "portable/MemMang/heap_4.c"
    "tasks.c"
    "list.c"
    "queue.c"
    "timers.c"
)

# Create a library target for FreeRTOS
add_library(freertos STATIC ${FREERTOS_SRC})

# Include directories for FreeRTOS
target_include_directories(freertos PUBLIC
    ${CMAKE_SOURCE_DIR}/freertos/include
    ${CMAKE_SOURCE_DIR}/freertos/source/include
)
