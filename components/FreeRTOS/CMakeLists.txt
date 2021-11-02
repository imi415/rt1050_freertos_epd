cmake_minimum_required(VERSION 3.10)
project(freertos-kernel
    LANGUAGES C ASM
)

if(NOT DEFINED FREERTOS_ARCH)
    message(FATAL_ERROR "Error: FREERTOS_ARCH not defined")
endif()

if(NOT DEFINED FREERTOS_TOOLCHAIN)
    message(FATAL_ERROR "Error: FREERTOS_TOOLCHAIN not defined")
endif()

if(NOT DEFINED FREERTOS_MEMMANG_METHOD)
    message(FATAL_ERROR "Error: FREERTOS_MEMMANG_METHOD not defined")
endif()

if(NOT FREERTOS_CONFIG_FILE)
    message(FATAL_ERROR "Error: FREERTOS_CONFIG_FILE not defined")
endif()

set(CUSTOM_INC_DIR ${CMAKE_CURRENT_BINARY_DIR}/custom_inc)
configure_file(${FREERTOS_CONFIG_FILE} ${CUSTOM_INC_DIR}/FreeRTOSConfig.h COPYONLY)

file(GLOB_RECURSE FREERTOS_PORTABLE_SOURCES
    "FreeRTOS-Kernel/portable/common/*.c"
    "FreeRTOS-Kernel/portable/MemMang/heap_${FREERTOS_MEMMANG_METHOD}.c"
    "FreeRTOS-Kernel/portable/${FREERTOS_TOOLCHAIN}/${FREERTOS_ARCH}/*.c"
    "FreeRTOS-Kernel/portable/${FREERTOS_TOOLCHAIN}/${FREERTOS_ARCH}/*.S"
    "FreeRTOS-Kernel/portable/${FREERTOS_TOOLCHAIN}/${FREERTOS_ARCH}/*.s"
)

set(FREERTOS_SOURCES
    "FreeRTOS-Kernel/croutine.c"
    "FreeRTOS-Kernel/event_groups.c"
    "FreeRTOS-Kernel/list.c"
    "FreeRTOS-Kernel/queue.c"
    "FreeRTOS-Kernel/stream_buffer.c"
    "FreeRTOS-Kernel/tasks.c"
    "FreeRTOS-Kernel/timers.c"
    ${FREERTOS_PORTABLE_SOURCES}
)

set(FREERTOS_INCLUDES
    "FreeRTOS-Kernel/include"
    "FreeRTOS-Kernel/portable/${FREERTOS_TOOLCHAIN}/${FREERTOS_ARCH}/"
)

add_library(${PROJECT_NAME} ${FREERTOS_SOURCES})

target_include_directories(${PROJECT_NAME}
    PUBLIC ${CUSTOM_INC_DIR}
    PUBLIC ${FREERTOS_INCLUDES}
)
