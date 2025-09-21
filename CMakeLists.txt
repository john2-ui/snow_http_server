cmake_minimum_required(VERSION 3.10)

project(snow_http_server VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 10)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

file(GLOB_RECURSE SOURCES
        ${PROJECT_SOURCE_DIR}/http/*.cpp
        ${PROJECT_SOURCE_DIR}/net/*.cpp
        ${PROJECT_SOURCE_DIR}/main.cpp

)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME}
        PRIVATE
        ${PROJECT_SOURCE_DIR}/http
        ${PROJECT_SOURCE_DIR}/net
        ${PROJECT_SOURCE_DIR}/coroutines)

find_package(Threads REQUIRED)
target_link_directories(${PROJECT_NAME} PRIVATE Threads::Threads)