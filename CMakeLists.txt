cmake_minimum_required(VERSION 3.24)
project(Server)

set(CMAKE_CXX_STANDARD 23)

add_executable(Server main.cpp coroutine_source.cpp 123.cpp check_response.cpp)
find_package(ZLIB)
target_link_libraries(Server ZLIB::ZLIB)

find_package(Boost 1.36.0 COMPONENTS iostreams)
include_directories(${Boost_INCLUDE_DIRS})
target_link_libraries(Server ${Boost_LIBRARIES})