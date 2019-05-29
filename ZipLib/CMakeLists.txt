cmake_minimum_required(VERSION 2.6)
project(ZipLib)

set (ZipLib_VERSION_MAJOR 1)
set (ZipLib_VERSION_MINOR 0)

configure_file (
    "${PROJECT_SOURCE_DIR}/ZipLibConfig.h.in"
    "${PROJECT_SOURCE_DIR}/include/ZipLibConfig.h"
)

include_directories("${PROJECT_SOURCE_DIR}")
include_directories("${PROJECT_SOURCE_DIR}/compression")
include_directories("${PROJECT_SOURCE_DIR}/compression/bzip2")
include_directories("${PROJECT_SOURCE_DIR}/compression/deflate")
include_directories("${PROJECT_SOURCE_DIR}/compression/lzma")
include_directories("${PROJECT_SOURCE_DIR}/compression/lzma/detail")
include_directories("${PROJECT_SOURCE_DIR}/compression/store")
include_directories("${PROJECT_SOURCE_DIR}/detail")
include_directories("${PROJECT_SOURCE_DIR}/methods")
include_directories("${PROJECT_SOURCE_DIR}/streams")
include_directories("${PROJECT_SOURCE_DIR}/streams/streambuffs")
include_directories("${PROJECT_SOURCE_DIR}/streams/utils")

file(GLOB SOURCES 
    "${PROJECT_SOURCE_DIR}/*.cpp" 
    "${PROJECT_SOURCE_DIR}/*.c" 
    "${PROJECT_SOURCE_DIR}/detail/*.cpp"
)

add_subdirectory("${PROJECT_SOURCE_DIR}/extlibs/bzip2")
add_subdirectory("${PROJECT_SOURCE_DIR}/extlibs/lzma")
add_subdirectory("${PROJECT_SOURCE_DIR}/extlibs/zlib")

add_library(ZipLib ${SOURCES})
target_link_libraries(ZipLib bzip2)
target_link_libraries(ZipLib lzma)
target_link_libraries(ZipLib zlib)