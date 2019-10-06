cmake_minimum_required (VERSION 3.9)
cmake_policy(SET CMP0048 NEW)
project (minimal_direct3d_example VERSION 1.0)


# use wWinMain/WinMain as entry point
set(CMAKE_WIN32_EXECUTABLE TRUE)

# library for setting dpi awareness
set(SHCORE_LIB shcore)

set(PROJECT_SOURCES "Minimal Direct3D Example/main.cpp")

add_executable(minimal_direct3d_example ${PROJECT_SOURCES})

target_link_libraries( minimal_direct3d_example ${SHCORE_LIB})

