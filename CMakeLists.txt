cmake_minimum_required(VERSION 3.24)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

project(chip8 C CXX)

file(GLOB_RECURSE SRC_FILES src/*.cpp src/*.c)

include(${CMAKE_CURRENT_SOURCE_DIR}/common_defs.cmake)

add_executable(chip8 ${SRC_FILES})

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/third_party/)
target_link_libraries(chip8 PUBLIC glad_loader)
target_link_libraries(chip8 PUBLIC glfw)

target_include_directories(chip8 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/)

