cmake_minimum_required(VERSION 2.6)
project(lzma)

set (lzma_VERSION_MAJOR 1)
set (lzma_VERSION_MINOR 0)

configure_file (
    "${PROJECT_SOURCE_DIR}/lzmaConfig.h.in"
    "${PROJECT_SOURCE_DIR}/include/lzmaConfig.h"
)

if (UNIX)

set(HEADERS 
    ${HEADERS}
    7z.h 
    7zAlloc.h
    7zFile.h
    Lzma86.h
    LzmaLib.h
)
include_directories("${PROJECT_SOURCE_DIR}/unix")
include_directories("${PROJECT_SOURCE_DIR}/unix/LzmaUtil")

file(GLOB SOURCES "${PROJECT_SOURCE_DIR}/unix/*.c")
set(SOURCES 
    ${SOURCES}
   7zAlloc.c
   7zDec.c
   7zFile.c
   7zIn.c
   LzmaLib.c
)

else()

include_directories("{PROJECT_SOURCE_DIR}")
file(GLOB SOURCES "${PROJECT_SOURCE_DIR}/*.c")

endif()



add_library(lzma ${SOURCES} ${HEADERS})