cmake_minimum_required(VERSION 3.29)
project(lab_1)

set(CMAKE_CXX_STANDARD 20)

add_executable(lab_1
        src/main.c
        headers/dialog.h
        src/converter.c
        headers/converter.h
        src/dialog.c)
