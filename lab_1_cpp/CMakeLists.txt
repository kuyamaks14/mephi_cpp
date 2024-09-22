cmake_minimum_required(VERSION 3.29)
project(lab_1_cpp)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(lab_1_cpp
        src/main.cpp
        headers/dialog.h
        src/converter.cpp
        headers/converter.h
        src/dialog.cpp)
