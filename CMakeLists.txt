cmake_minimum_required(VERSION 3.10)
project(FastBlur)

# EasyBMP
add_subdirectory(lib/EasyBMP)
include_directories(lib/EasyBMP)
#link_directories(lib/EasyBMP)

add_executable(FastBlur FastBlur.cpp)
target_link_libraries(FastBlur EasyBMP)
