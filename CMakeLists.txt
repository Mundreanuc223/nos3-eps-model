cmake_minimum_required(VERSION 3.26)
project(eps_wrapper)

set(CMAKE_CXX_STANDARD 17)

include_directories(.)

add_executable(eps_wrapper
        eps-model-wrapper.cpp
        eps-model-wrapper.h
        helper-functions.h
        wrapper-tests.cpp)
