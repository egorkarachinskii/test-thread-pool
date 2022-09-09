cmake_minimum_required(VERSION 2.8)

project(threadPool)

set(CMAKE_CXX_STANDARD 11)
set(HEADER threadpool.h)

add_executable(${PROJECT_NAME} "main.cpp")

target_link_libraries(${PROJECT_NAME} pthread)
