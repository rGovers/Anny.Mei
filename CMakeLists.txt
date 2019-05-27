cmake_minimum_required (VERSION 2.6)
project (Application)

set (Application_VERSION_MAJOR 1)
set (Application_VERSION_MINOR 0)

configure_file (
    "${PROJECT_SOURCE_DIR}/include/ApplicationConfig.h.in"
    "${PROJECT_BINARY_DIR}/ApplicationConfig.h"
)

include_directories("${PROJECT_SOURCE_DIR}/include")

add_executable (Application main.cpp)