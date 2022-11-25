cmake_minimum_required(VERSION 3.1)
project(kmp_cpu LANGUAGES CXX)
include_directories("../common")
add_executable(${PROJECT_NAME} main.cpp)
