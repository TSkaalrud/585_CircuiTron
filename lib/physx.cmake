include(FetchContent)
FetchContent_Declare(
	physx
	GIT_REPOSITORY https://github.com/phcerdan/PhysX
	GIT_TAG ae80ded
	PATCH_COMMAND git apply ${CMAKE_SOURCE_DIR}/lib/physx.diff || true
)
FetchContent_GetProperties(physx)
if(NOT physx_POPULATED)
	FetchContent_Populate(physx)
	
	if(UNIX)
		set(TARGET_BUILD_PLATFORM "linux")
		set(CMAKE_LIBRARY_ARCHITECTURE "x86_64-linux-gnu")
	else()
		set(TARGET_BUILD_PLATFORM "windows")
		SET(CMAKE_EXE_LINKER_FLAGS_PROFILE "")
		SET(CMAKE_EXE_LINKER_FLAGS_CHECKED "")
	endif()

	set(NV_USE_DEBUG_WINCRT ON CACHE BOOL "Use the debug version of the CRT")
	set(PHYSX_ROOT_DIR ${physx_SOURCE_DIR}/physx)
	set(PXSHARED_PATH ${PHYSX_ROOT_DIR}/../pxshared)
	set(PXSHARED_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})
	set(CMAKEMODULES_VERSION "1.27")
	set(CMAKEMODULES_PATH ${PHYSX_ROOT_DIR}/../externals/cmakemodules)
	set(PX_OUTPUT_LIB_DIR ${CMAKE_CURRENT_BINARY_DIR}/externals/physx)
	set(PX_OUTPUT_BIN_DIR ${CMAKE_CURRENT_BINARY_DIR}/externals/physx)
	if (MSVC)
	else()
		add_compile_options(-w)
	endif()

	add_subdirectory(${PHYSX_ROOT_DIR}/compiler/public externals/physx)
endif()

target_link_libraries(libs INTERFACE PhysX PhysXCommon PhysXExtensions PhysXCooking)