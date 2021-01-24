cmake_minimum_required(VERSION 3.17)

project(circuitron LANGUAGES CXX)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_LIST_DIR}/src/*.cpp)
add_executable(circuitron ${SOURCES})
target_include_directories(circuitron PRIVATE ${CMAKE_CURRENT_LIST_DIR}/src/)
install(TARGETS circuitron DESTINATION .)

set_property(TARGET circuitron PROPERTY CXX_STANDARD 20)
set_property(TARGET circuitron PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET circuitron PROPERTY CXX_EXTENSIONS OFF)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

if (MSVC)
	target_compile_options(circuitron PUBLIC /W4)
else()
	target_compile_options(circuitron PUBLIC -Wall -Wextra -pedantic)
endif()

include(lib/lib.cmake)

add_custom_target(shaders
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/shaders ${CMAKE_CURRENT_BINARY_DIR}/shaders
)
add_dependencies(circuitron shaders)
install(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/shaders DESTINATION .)

add_custom_target(assets
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/assets ${CMAKE_CURRENT_BINARY_DIR}/assets
)
add_dependencies(circuitron assets)
install(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/assets DESTINATION .)

target_link_libraries(circuitron libs)
