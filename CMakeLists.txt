cmake_minimum_required(VERSION 3.14)
project(weather)

set(CMAKE_CXX_STANDARD 17)

find_package(CURL REQUIRED)

add_executable(weather weather.cpp)
target_link_libraries(weather PRIVATE CURL::libcurl)
