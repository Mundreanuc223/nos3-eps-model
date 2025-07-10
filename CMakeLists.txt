cmake_minimum_required(VERSION 3.26)
project(eps-wrapper)

set(CMAKE_CXX_STANDARD 17)

add_executable(eps-wrapper wrapper-tests.cpp
eps-model-wrapper.cpp)