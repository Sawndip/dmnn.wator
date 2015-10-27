cmake_minimum_required(VERSION 2.8)
project(wator)
include_directories(include)

include_directories(/usr/local/Cellar/boost/1.58.0/include)
include_directories(/usr/local/Cellar/opencv/2.4.12/include)
link_directories(/usr/local/Cellar/boost/1.58.0/lib)
link_directories(/usr/local/Cellar/opencv/2.4.12/lib)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11 -g")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g")

add_subdirectory(src)
add_subdirectory(test)
add_subdirectory(tools)
