cmake_minimum_required(VERSION 3.10)

project(EasyBMP VERSION 1.0)

include_directories(.)

file(GLOB SOURCES "./*.cpp")

add_library(EasyBMP STATIC ${SOURCES})
