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

add_compile_definitions(IMGUI_IMPL_OPENGL_LOADER_GLAD)

# Allows me to write GLSL shaders as seperate files 
# and not have to go through the effort of file IO

FileToHeader(
    SOURCE_FILE "shaders/ViewVertex.glsl"
    HEADER_FILE "include/Shaders/ViewVertex.h"
    VARIABLE_NAME "ViewVertex")
FileToHeader(
    SOURCE_FILE "shaders/ModelVertex.glsl" 
    HEADER_FILE "include/Shaders/ModelVertex.h"
    VARIABLE_NAME "ModelVertex")
FileToHeader(
    SOURCE_FILE "shaders/MorphPlaneVertex.glsl"
    HEADER_FILE "include/Shaders/MorphPlaneVertex.h"
    VARIABLE_NAME "MorphPlaneVertex")
FileToHeader(
    SOURCE_FILE "shaders/MorphTargetVertex.glsl"
    HEADER_FILE "include/Shaders/MorphTargetVertex.h"
    VARIABLE_NAME "MorphTargetVertex")

FileToHeader(
    SOURCE_FILE "shaders/StandardPixel.glsl" 
    HEADER_FILE "include/Shaders/StandardPixel.h"
    VARIABLE_NAME "StandardPixel")
FileToHeader(
    SOURCE_FILE "shaders/SolidPixelColor.glsl"
    HEADER_FILE "include/Shaders/SolidPixelColor.h"
    VARIABLE_NAME "SolidPixelColor")
FileToHeader(
    SOURCE_FILE "shaders/SolidPixel.glsl" 
    HEADER_FILE "include/Shaders/SolidPixel.h"
    VARIABLE_NAME "SolidPixel")

include_directories("${PROJECT_SOURCE_DIR}")
include_directories("${PROJECT_SOURCE_DIR}/include")
include_directories("${PROJECT_SOURCE_DIR}/glad")
include_directories("${PROJECT_SOURCE_DIR}/glm")
include_directories("${PROJECT_SOURCE_DIR}/imgui")
include_directories("${PROJECT_SOURCE_DIR}/miniz-2.1.0")
include_directories("${PROJECT_SOURCE_DIR}/stb")
include_directories("${PROJECT_SOURCE_DIR}/tinyfiledialogs")
file(GLOB SOURCES "src/*.cpp" "miniz-2.1.0/*.c" "imgui/*.cpp" "glad/*.c" "tinyfiledialogs/*.c")

add_subdirectory("glfw-3.3")
include_directories("glfw-3.3/include")

add_executable (Application ${SOURCES})
target_link_libraries(Application glfw ${GLFW_LIBRARIES})