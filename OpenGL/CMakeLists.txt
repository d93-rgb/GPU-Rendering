cmake_minimum_required (VERSION 3.9)
cmake_policy(SET CMP0048 NEW)
project(minimal_opengl_exmaple VERSION 1.0)
set(CMAKE_CXX_STANDARD 17)

option(BUILD_SHARED_LIBS "Build shared libs for all libraries" ON) 

if(NOT MSVC)
	SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -std=gnu++11")
	SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -ldl")
else(NOT MSVC)
	# important flag, for __FILE__ macro to have absolute path 
	SET(COMP_FLAGS "/FC")
	SET(CMAKE_CXX_FLAGS "${COMP_FLAGS}")
	# alternatively, add the following line after add_executable
	#target_compile_options(minimal_opengl_example PUBLIC /FC)
endif(NOT MSVC)

###########################################################################
# OpenGL
find_package(OpenGL REQUIRED)
if( NOT OPENGL_FOUND )
	message(FATAL_ERROR 
		"OpenGL library could not be found on this system.")
endif()

###########################################################################

###########################################################################
# glfw
# build as static library - so no dll
add_subdirectory(ext/glfw)
include_directories (
  ext/glfw/include
)
###########################################################################

###########################################################################
# glew
# build as dynamic library
#add_subdirectory(ext/glew-2.1.0/build/cmake)
#include_directories (
#  ext/glew-2.1.0/include/GL
#)
###########################################################################

# gl3w 
###########################################################################
include_directories(ext/gl3w)
set(GL3W_SOURCES ext/gl3w/GL/gl3w.c)
###########################################################################

set(ALL_PROJECT_LIBRARIES
    OpenGL::GL
	OpenGL::GLU
	glfw)	# <- target name, contains absolute path to library

# print search paths for libraries
#get_property(test_LINK_DIRECTORIES DIRECTORY PROPERTY LINK_DIRECTORIES)
#message(${test_LINK_DIRECTORIES})

SET(PROJECT_SOURCES "Minimal OpenGL Example/main.cpp")

add_executable(minimal_opengl_example ${OPTIONS} ${PROJECT_SOURCES} ${GL3W_SOURCES})

target_link_libraries(minimal_opengl_example ${ALL_PROJECT_LIBRARIES})

#if(WIN32)
	# post build command for copying dll files
#	set (GLEW_DLL_DIR "${CMAKE_BINARY_DIR}/bin/")
	
#	if(CMAKE_BUILD_TYPE STREQUAL "Debug")
#		set(GLEW_DLL_NAME glew32d)
#	else()
#		set(GLEW_DLL_NAME glew32)
#	endif()
	
#	add_custom_command(TARGET minimal_opengl_example POST_BUILD
#		COMMAND ${CMAKE_COMMAND} -E copy_if_different
#			"${GLEW_DLL_DIR}/${GLEW_DLL_NAME}.dll"
#			$<TARGET_FILE_DIR:minimal_opengl_example>)
#endif()

