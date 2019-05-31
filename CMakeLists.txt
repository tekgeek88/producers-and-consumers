cmake_minimum_required(VERSION 3.14)
project(HW_02___pcMatrix C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(HW_02___pcMatrix
        counter.c
        counter.h
        matrix.c
        matrix.h
        pcmatrix.c
        pcmatrix.h
        prodcons.c
        prodcons.h)
