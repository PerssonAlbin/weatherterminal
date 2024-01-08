cmake_minimum_required(VERSION 3.14)
project(weather)

set(CMAKE_CXX_STANDARD 17)
include(FetchContent)

find_package(CURL REQUIRED)

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz)
FetchContent_MakeAvailable(json)

add_executable(weather weather.cpp)

target_link_libraries(weather PRIVATE CURL::libcurl)
target_link_libraries(weather PRIVATE nlohmann_json::nlohmann_json)
