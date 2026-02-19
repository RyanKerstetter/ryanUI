
cmake_minimum_required(VERSION 3.16)

project(ryanUI)

set(CMAKE_CXX_STANDARD 17)

file(GLOB SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_executable(ryanUI ${SOURCES})

find_package(raylib REQUIRED)

target_link_libraries(${PROJECT_NAME} PRIVATE raylib)
