cmake_minimum_required(VERSION 3.10)
project(lvgl
    LANGUAGES C ASM
)

if(NOT LVGL_CONFIG_FILE)
    message(FATAL_ERROR "Error: LVGL_CONFIG_FILE not defined")
endif()

set(CUSTOM_INC_DIR ${CMAKE_CURRENT_BINARY_DIR}/custom_inc)
configure_file(${FREERTOS_CONFIG_FILE} ${CUSTOM_INC_DIR}/lv_conf.h COPYONLY)

file(GLOB_RECURSE LVGL_SOURCES
    "lvgl/src/*.c"
)

set(LVGL_INCLUDES
    "lvgl"
)

add_library(${PROJECT_NAME} ${LVGL_SOURCES})

target_include_directories(${PROJECT_NAME}
    PUBLIC ${CUSTOM_INC_DIR}
    PUBLIC ${LVGL_INCLUDES}
)
