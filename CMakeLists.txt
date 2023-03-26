cmake_minimum_required(VERSION 3.24)
project(Server)

set(CMAKE_CXX_STANDARD 23)

add_executable(Server main.cpp coroutine_source.cpp 123.cpp)
