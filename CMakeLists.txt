cmake_minimum_required(VERSION 4.1)
project(High_Frequency_Trading_Challenge)

set(CMAKE_CXX_STANDARD 20)

add_executable(High_Frequency_Trading_Challenge main.cpp
        Profiler.h
        Parser.h)
