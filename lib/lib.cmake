include(FetchContent)

add_library(libs INTERFACE)

find_package(glfw3 3.3.2 QUIET)
if(NOT TARGET glfw)
	FetchContent_Declare(GLFW
		URL https://github.com/glfw/glfw/releases/download/3.3.2/glfw-3.3.2.zip
	)
	FetchContent_GetProperties(GLFW)
	if(NOT glfw_POPULATED)
		FetchContent_Populate(GLFW)
		set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
		set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
		set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
		set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
		add_subdirectory(${glfw_SOURCE_DIR} EXCLUDE_FROM_ALL)
	endif()
endif()
target_compile_definitions(libs INTERFACE GLFW_INCLUDE_NONE)
target_link_libraries(libs INTERFACE glfw)

include(lib/gl/gl.cmake)
target_link_libraries(libs INTERFACE gl)

include(lib/physx.cmake)