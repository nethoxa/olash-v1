cmake_minimum_required(VERSION 3.25)
project(phviewer_vs)

set(CMAKE_CXX_STANDARD 14)

find_package(OpenGL REQUIRED)

include_directories(.)
include_directories(glad/glad)
include_directories(glad/KHR)
include_directories(util)

add_executable(phviewer_vs
        glad/glad/glad.h
        glad/KHR/khrplatform.h
        glad/glad.cpp
        shaders/fpassthrough.glsl
        shaders/vpassthrough.glsl
        util/GLFWWindow.cpp
        util/GLFWWindow.h
        util/lodepng.cpp
        util/lodepng.h
        util/tb.cpp
        util/tb.h
        util/tiny_obj_loader.cpp
        util/tiny_obj_loader.h
        util/trackball.cpp
        util/trackball.h
        PHViewer.cpp
        PHViewer.h PHViewer.cpp)

target_compile_definitions(phviewer_vs PRIVATE
        SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}/")

target_link_libraries(phviewer_vs
        glfw
        OpenGL::GL)
