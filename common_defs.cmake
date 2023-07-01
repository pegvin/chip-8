# Some Common Definitions...

# Debug C/C++ flags
if(CMAKE_BUILD_TYPE STREQUAL Debug)
	add_compile_options(-fsanitize=address)
	add_link_options(-fsanitize=address)
	add_definitions(-D_DEBUG)
else()
	add_definitions(-D_NDEBUG)
endif()
