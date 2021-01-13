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

find_package(assimp 5.0.1 QUIET)
if(TARGET assimp::assimp)
	target_link_libraries(libs INTERFACE ${ASSIMP_LIBRARIES})
else()
	FetchContent_Declare(ASSIMP
		URL https://github.com/assimp/assimp/archive/v5.0.1.zip
	)
	FetchContent_GetProperties(ASSIMP)
	if(NOT assimp_POPULATED)
		FetchContent_Populate(ASSIMP)
		add_subdirectory(${assimp_SOURCE_DIR} EXCLUDE_FROM_ALL)
	endif()

	target_link_libraries(libs INTERFACE assimp)
endif()

find_package(glm 0.9.9.8 QUIET)
if(NOT TARGET glm)
	FetchContent_Declare(GLM
		URL https://github.com/g-truc/glm/releases/download/0.9.9.8/glm-0.9.9.8.zip
	)
	FetchContent_GetProperties(GLM)
	if(NOT glm_POPULATED)
		FetchContent_Populate(GLM)
		add_subdirectory(${glm_SOURCE_DIR} EXCLUDE_FROM_ALL)
	endif()
endif()
target_link_libraries(libs INTERFACE glm)

# add_subdirectory(lib/PhysX/physx EXCLUDE_FROM_ALL)
# target_link_libraries(libs INTERFACE PhysX PhysXCommon PhysXExtensions PhysXCooking)
# if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	# target_link_libraries(libs INTERFACE PhysXPvdSDK)
# endif()
