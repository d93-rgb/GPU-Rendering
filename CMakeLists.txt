cmake_minimum_required (VERSION 3.9)
cmake_policy(SET CMP0048 NEW)
project(ShaderCompilationExample VERSION 1.0)

if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
	message(STATUS "*** Setting build type to 'Release' ***")
	set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
	set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release"
    "MinSizeRel" "RelWithDebInfo")
elseif (NOT CMAKE_BUILD_TYPE)
	message(STATUS "*** Setting build type to 'Release' ***")
	set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
else()
	message(STATUS "*** ${CMAKE_BUILD_TYPE} build ***")
endif()


if(MSVC)
	message(STATUS "Adding Direct3d example...")
	add_subdirectory(Direct3D)
endif(MSVC)

message(STATUS "Adding OpenGL example...")
add_subdirectory(OpenGL)
