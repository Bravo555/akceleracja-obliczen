cmake_minimum_required(VERSION 3.1) # 3.1 << C_STANDARD 11

project(kmp_gpu LANGUAGES CXX)

include_directories("../common")

find_package(OpenCL REQUIRED)

add_executable(${PROJECT_NAME} Main.cpp)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/string_search_kernel.cl ${CMAKE_CURRENT_BINARY_DIR}/string_search_kernel.cl COPYONLY)

target_link_libraries(${PROJECT_NAME} PRIVATE OpenCL::OpenCL)

set_target_properties(${PROJECT_NAME} PROPERTIES C_STANDARD 11
                                                 C_STANDARD_REQUIRED ON
                                                 C_EXTENSIONS OFF)

target_compile_definitions(${PROJECT_NAME} PRIVATE CL_TARGET_OPENCL_VERSION=100)

