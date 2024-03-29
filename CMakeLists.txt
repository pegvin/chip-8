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

# Debug C/C++ flags
if(CMAKE_BUILD_TYPE STREQUAL Debug)
	add_compile_options(-fsanitize=address)
	add_link_options(-fsanitize=address)
	add_definitions(-D_DEBUG)
else()
	add_definitions(-D_NDEBUG)
endif()

add_executable(chip8 ${SRC_FILES})
target_include_directories(chip8 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/)
target_compile_definitions(chip8 PUBLIC _CRT_SECURE_NO_WARNINGS SDL_MAIN_HANDLED=1)
find_package(SDL2 REQUIRED CONFIG REQUIRED COMPONENTS SDL2)

# Prefer Linking With SDL2 Statically But If Not Available, Link Dynamically.
if(TARGET SDL2::SDL2-static)
	target_link_libraries(chip8 PUBLIC SDL2::SDL2-static)
	message(STATUS "Linking With SDL2 Statically.")
elseif(TARGET SDL2::SDL2)
	target_link_libraries(chip8 PUBLIC SDL2::SDL2)
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

target_include_directories(chip8 PUBLIC ${SDL_INCLUDE_DIRS})

