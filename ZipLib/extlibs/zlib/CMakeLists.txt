cmake_minimum_required(VERSION 2.6)
project(zlib)

set (zlib_VERSION_MAJOR 1)
set (zlib_VERSION_MINOR 0)

configure_file (
    "${PROJECT_SOURCE_DIR}/zlibConfig.h.in"
    "${PROJECT_SOURCE_DIR}/include/zlibConfig.h"
)

include_directories("${PROJECT_SOURCE_DIR}")
file(GLOB SOURCES "${PROJECT_SOURCE_DIR}/*.c")

add_library(zlib ${SOURCES})