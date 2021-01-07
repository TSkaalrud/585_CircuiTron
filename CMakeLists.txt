cmake_minimum_required(VERSION 3.17)

project(circuitron LANGUAGES CXX)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_LIST_DIR}/src/*.cpp)
add_executable(circuitron ${SOURCES})

set_property(TARGET circuitron PROPERTY CXX_STANDARD 20)
set_property(TARGET circuitron PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET circuitron PROPERTY CXX_EXTENSIONS OFF)

if (MSVC)
	target_compile_options(circuitron PUBLIC /W4)
else()
	target_compile_options(circuitron PUBLIC -Wall -Wextra -pedantic)
endif()

include(lib/lib.cmake)

target_link_libraries(circuitron libs)
