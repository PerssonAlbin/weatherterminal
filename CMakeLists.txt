cmake_minimum_required(VERSION 3.24)
project(weather LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

find_package(CURL REQUIRED)

set(CMAKE_INSTALL_PREFIX "/usr/local")

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz)
FetchContent_MakeAvailable(json)

add_executable(weather weather.cpp)

target_link_libraries(weather PRIVATE CURL::libcurl)
target_link_libraries(weather PRIVATE nlohmann_json::nlohmann_json)

install(TARGETS weather
	RUNTIME DESTINATION bin)
