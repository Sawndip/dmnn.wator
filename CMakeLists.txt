cmake_minimum_required(VERSION 2.8)
project(wator)
include_directories(include)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11 -g")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g")

add_subdirectory(src)
add_subdirectory(test)
add_subdirectory(tools)
