cmake_minimum_required(VERSION 3.3)
project(teramind)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp)
add_executable(teramind ${SOURCE_FILES})
add_executable(ImageRle ImageRle.cpp)