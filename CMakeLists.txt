cmake_minimum_required(VERSION 3.26)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS OFF)
            
project(app)
            
add_executable(runnable main.cpp)

include(CMakePrintHelpers)

cmake_print_variables(CMAKE_CURRENT_BINARY_DIR)