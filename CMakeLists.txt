cmake_minimum_required (VERSION 2.6)
project (Application)

set (Application_VERSION_MAJOR 1)
set (Application_VERSION_MINOR 0)

configure_file (
    "${PROJECT_SOURCE_DIR}/ApplicationConfig.h.in"
    "${PROJECT_SOURCE_DIR}/include/ApplicationConfig.h"
)

include_directories("${PROJECT_SOURCE_DIR}/include")
file(GLOB SOURCES "src/*.cpp")

add_executable (Application ${SOURCES})