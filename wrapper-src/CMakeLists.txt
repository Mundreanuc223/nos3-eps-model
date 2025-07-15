cmake_minimum_required(VERSION 3.15)
project(eps_wrapper)
set(CMAKE_CXX_STANDARD 17)

add_executable(eps_wrapper
        wrapper-tests.cpp
        eps-model-wrapper.cpp
        eps-model-wrapper.h
        helper-functions.h
)