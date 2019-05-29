cmake_minimum_required (VERSION 2.6)
project (Application)

set (Application_VERSION_MAJOR 1)
set (Application_VERSION_MINOR 0)

configure_file (
    "${PROJECT_SOURCE_DIR}/ApplicationConfig.h.in"
    "${PROJECT_SOURCE_DIR}/include/ApplicationConfig.h"
)

include_directories("${PROJECT_SOURCE_DIR}")
include_directories("${PROJECT_SOURCE_DIR}/include")
include_directories("${PROJECT_SOURCE_DIR}/imgui")
include_directories("${PROJECT_SOURCE_DIR}/glad")
file(GLOB SOURCES "src/*.cpp"  "imgui/*.cpp" "glad/*.c")

add_subdirectory("glfw-3.3")
include_directories("glfw-3.3/include")

add_subdirectory("ZipLib")

add_executable (Application ${SOURCES})
target_link_libraries(Application glfw ${GLFW_LIBRARIES})
target_link_libraries(Application ZipLib)