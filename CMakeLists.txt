cmake_minimum_required(VERSION 3.1)
project(Newdark_Brighter)

set(CMAKE_CXX_STANDARD 20)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

add_executable(Newdark_Brighter main.cpp)
target_link_libraries(Newdark_Brighter ${SDL2_LIBRARIES})
