cmake_minimum_required (VERSION 3.9)
cmake_policy(SET CMP0048 NEW)
project(minimal_opengl_exmaple VERSION 1.0)

if(MSVC)
	message("Adding Direct3d example...")
	add_subdirectory(Direct3D)
endif(MSVC)

message("Adding OpenGL example...")
add_subdirectory(OpenGL)
