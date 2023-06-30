cmake_minimum_required(VERSION 3.24)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Use GNU Extensions Of Non-Apple Devices Because Of Gunslinger
if (APPLE)
    set(CMAKE_C_EXTENSIONS OFF)
else()
    set(CMAKE_C_EXTENSIONS ON)
endif()

project(chip8 C)

file(GLOB_RECURSE SRC_FILES src/*.cpp src/*.c)
include(${CMAKE_CURRENT_SOURCE_DIR}/common_defs.cmake)

add_executable(chip8 ${SRC_FILES})

if (NOT APPLE AND NOT WINDOWS)
    target_link_libraries(chip8 m)
endif()

find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})
target_link_libraries(chip8 ${OPENGL_LIBRARIES})

target_include_directories(chip8 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/)
target_include_directories(chip8 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/gunslinger/)

