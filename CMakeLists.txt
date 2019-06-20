cmake_minimum_required (VERSION 2.6)
project (Application)

set (Application_VERSION_MAJOR 1)
set (Application_VERSION_MINOR 0)

configure_file (
    "${PROJECT_SOURCE_DIR}/ApplicationConfig.h.in"
    "${PROJECT_SOURCE_DIR}/include/ApplicationConfig.h"
)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/modules")

include("FileToHeader")

# Allows me to write GLSL shaders as seperate files 
# and not have to go through the effort of file IO
FILETOHEADER(
    SOURCE_FILE "shaders/test.glsl" 
    HEADER_FILE "include/Shaders/testShader.h" 
    VARIABLE_NAME "test")

include_directories("${PROJECT_SOURCE_DIR}")
include_directories("${PROJECT_SOURCE_DIR}/include")
include_directories("${PROJECT_SOURCE_DIR}/glad")
include_directories("${PROJECT_SOURCE_DIR}/imgui")
include_directories("${PROJECT_SOURCE_DIR}/rapidxml-1.13")
include_directories("${PROJECT_SOURCE_DIR}/stb")
include_directories("${PROJECT_SOURCE_DIR}/tinyfiledialogs")
file(GLOB SOURCES "src/*.cpp" "imgui/*.cpp" "glad/*.c" "tinyfiledialogs/*.c")

add_subdirectory("glfw-3.3")
include_directories("glfw-3.3/include")

add_subdirectory("ZipLib")

add_executable (Application ${SOURCES})
target_link_libraries(Application glfw ${GLFW_LIBRARIES})
target_link_libraries(Application ZipLib)