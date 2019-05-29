cmake_minimum_required(VERSION 2.6)
project(bzip2 C)

set (bzip2_VERSION_MAJOR 1)
set (bzip2_VERSION_MINOR 0)

configure_file (
    "${PROJECT_SOURCE_DIR}/bzip2Config.h.in"
    "${PROJECT_SOURCE_DIR}/include/bzip2Config.h"
)

include_directories("{PROJECT_SOURCE_DIR}")
file(GLOB SOURCES 
    "${PROJECT_SOURCE_DIR}/*.c" 
)

add_library(bzip2 ${SOURCES})