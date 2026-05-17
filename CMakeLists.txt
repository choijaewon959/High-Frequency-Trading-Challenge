cmake_minimum_required(VERSION 4.1)
project(High_Frequency_Trading_Challenge)

set(CMAKE_CXX_STANDARD 20)

add_executable(client main.cpp
        Profiler.h
        Parser.h
        mm.h
)
add_executable(server server.cpp
        json.hpp
)