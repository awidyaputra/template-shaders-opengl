# Copyright (c) 2016, NVIDIA CORPORATION.  All rights reserved.
cmake_minimum_required (VERSION 3.23.2)
project(template-shaders-opengl C CXX)

include(FetchContent)
include(CMakePrintHelpers)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/install)

FetchContent_Declare(imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG e25e4526cd41cd6536194de098540d54244f54e9 # v1.89.3 - Docking branch
)

FetchContent_Declare(glfw
    URL "https://github.com/glfw/glfw/archive/refs/tags/3.3.8.tar.gz"
    CMAKE_ARGS
        -DGLFW_BUILD_EXAMPLES OFF
        -DGLFW_BUILD_TESTS OFF
        -DGLFW_BUILD_DOCS OFF
)

FetchContent_MakeAvailable(imgui glfw)

cmake_print_variables(imgui_SOURCE_DIR)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

#-------------------------------------------------------------------------------
# System packages
#-------------------------------------------------------------------------------
find_package(Threads REQUIRED)

#-------------------------------------------------------------------------------
# Project files
#-------------------------------------------------------------------------------
set(SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/glad2-opengl430/src/glad.c
  ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
  ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
  ${imgui_SOURCE_DIR}/imgui.cpp
  ${imgui_SOURCE_DIR}/imgui_draw.cpp
  ${imgui_SOURCE_DIR}/imgui_tables.cpp
  ${imgui_SOURCE_DIR}/imgui_widgets.cpp
  ${imgui_SOURCE_DIR}/imgui_demo.cpp

  ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/ui.cpp
  )
#-------------------------------------------------------------------------------
# Final target
#-------------------------------------------------------------------------------
add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/glad2-opengl430/include
    ${imgui_SOURCE_DIR}/backends
    ${imgui_SOURCE_DIR}
    )
target_link_libraries(${PROJECT_NAME} PRIVATE
    glfw
    GL
    )

install(TARGETS ${PROJECT_NAME})
