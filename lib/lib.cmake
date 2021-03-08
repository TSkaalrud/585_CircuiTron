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

include(${CMAKE_CURRENT_LIST_DIR}/gl/gl.cmake)
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
		set(ASSIMP_NO_EXPORT ON CACHE BOOL "" FORCE)
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

add_library(physx INTERFACE)
target_include_directories(physx INTERFACE ${CMAKE_CURRENT_LIST_DIR}/physx/include/)
target_link_directories(physx INTERFACE ${CMAKE_CURRENT_LIST_DIR}/physx/${CMAKE_SYSTEM_NAME}/${CMAKE_BUILD_TYPE}/)
target_link_libraries(physx INTERFACE PhysXPvdSDK_static_64)
target_link_libraries(physx INTERFACE 
	PhysXExtensions_static_64
	PhysXVehicle_static_64
	PhysX_64
	PhysXCommon_64
	PhysXCooking_64
	PhysXFoundation_64
)
target_link_libraries(libs INTERFACE physx)
if(UNIX)
file(GLOB_RECURSE SOS CONFIGURE_DEPENDS ${CMAKE_CURRENT_LIST_DIR}/physx/${CMAKE_SYSTEM_NAME}/${CMAKE_BUILD_TYPE}/*.so)
install(FILES ${SOS} DESTINATION .)
else()
file(GLOB_RECURSE DLLS CONFIGURE_DEPENDS ${CMAKE_CURRENT_LIST_DIR}/physx/${CMAKE_SYSTEM_NAME}/${CMAKE_BUILD_TYPE}/*.dll)
file(COPY ${DLLS} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
install(FILES ${DLLS} DESTINATION .)
endif()

add_library(stb INTERFACE)
target_include_directories(stb INTERFACE ${CMAKE_CURRENT_LIST_DIR}/stb/)
target_link_libraries(libs INTERFACE stb)

file(GLOB_RECURSE IMGUI_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_LIST_DIR}/imgui/*.cpp)
add_library(imgui ${IMGUI_SOURCES})
target_include_directories(imgui PUBLIC ${CMAKE_CURRENT_LIST_DIR}/imgui/)
target_link_libraries(imgui gl glfw)
target_compile_definitions(imgui PRIVATE IMGUI_IMPL_OPENGL_LOADER_CUSTOM="gl.hpp")
target_link_libraries(libs INTERFACE imgui)

find_package(OpenAL CONFIG QUIET)
if(NOT TARGET OpenAL::OpenAL)
	SET(LIBTYPE STATIC)
	FetchContent_Declare(OPENAL
		URL https://github.com/kcat/openal-soft/archive/1.21.1.zip
	)
	FetchContent_GetProperties(OPENAL)
	if(NOT openal_POPULATED)
		FetchContent_Populate(OPENAL)
		add_subdirectory(${openal_SOURCE_DIR} EXCLUDE_FROM_ALL)
	endif()
	target_link_libraries(libs INTERFACE OpenAL)
else()
	target_link_libraries(libs INTERFACE OpenAL::OpenAL)
endif()
