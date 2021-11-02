cmake_minimum_required(VERSION 3.10)

project(epd)

set(EPD_SOURCES
    "src/epd_common.c"
    "src/epd_wfh0420cz35.c"
)

set(EPD_INCLUDES
    "include"
)

add_library(${PROJECT_NAME} ${EPD_SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC ${EPD_INCLUDES})