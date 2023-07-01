cmake_minimum_required(VERSION 3.19)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

project(chip8 C)

file(
	GLOB SRC_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/src/main.c
	${CMAKE_CURRENT_SOURCE_DIR}/src/system.c
	${CMAKE_CURRENT_SOURCE_DIR}/src/win.c
)

include(${CMAKE_CURRENT_SOURCE_DIR}/common_defs.cmake)
add_executable(chip8 ${SRC_FILES})
target_include_directories(chip8 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/)
find_package(SDL2 REQUIRED CONFIG REQUIRED COMPONENTS SDL2)

# Prefer Linking With SDL2 Statically But If Not Available, Link Dynamically.
if(TARGET SDL2::SDL2-static)
	target_link_libraries(chip8 PRIVATE SDL2::SDL2-static)
	message(STATUS "Linking With SDL2 Statically.")
elseif(TARGET SDL2::SDL2)
	target_link_libraries(chip8 PRIVATE SDL2::SDL2)
	# When Linking Dynamically, Make Sure To Copy .dll Files
	if(WIN32)
		add_custom_command(TARGET csprite POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy -t $<TARGET_FILE_DIR:csprite> $<TARGET_RUNTIME_DLLS:csprite>
			COMMAND_EXPAND_LISTS
		)
	endif()

	message(STATUS "Linking With SDL2 Dynamically.")
else()
	message(FATAL_ERROR "neither static nor dynamic library of SDL2 found!")
endif()
