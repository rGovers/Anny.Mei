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
    SOURCE_FILE "shaders/MorphTarget9PointVertex.glsl"
    HEADER_FILE "include/Shaders/MorphTarget9PointVertex.h"
    VARIABLE_NAME "MorphTarget9PointVertex")

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
include_directories("${PROJECT_SOURCE_DIR}/lib")
include_directories("${PROJECT_SOURCE_DIR}/lib/glad")
include_directories("${PROJECT_SOURCE_DIR}/lib/glm")
include_directories("${PROJECT_SOURCE_DIR}/lib/imgui")
include_directories("${PROJECT_SOURCE_DIR}/lib/liblzf-3.6")
include_directories("${PROJECT_SOURCE_DIR}/lib/miniz-2.1.0")
include_directories("${PROJECT_SOURCE_DIR}/lib/stb")
include_directories("${PROJECT_SOURCE_DIR}/lib/tinyfiledialogs")
file(GLOB SOURCES "src/*.cpp" "lib/miniz-2.1.0/*.c" "lib/imgui/*.cpp" "lib/glad/*.c" "lib/tinyfiledialogs/*.c" "lib/liblzf-3.6/*.c")

add_subdirectory("lib/glfw-3.3")
include_directories("lib/glfw-3.3/include")

add_executable (Application ${SOURCES})
target_link_libraries(Application glfw ${GLFW_LIBRARIES})