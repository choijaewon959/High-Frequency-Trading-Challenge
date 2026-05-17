cmake_minimum_required(VERSION 3.20)
project(High_Frequency_Trading_Challenge LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(USE_OPENMP "Enable OpenMP acceleration" ON)

add_executable(client
        main.cpp
        Profiler.h
        Parser.h
        mm.h
)

add_executable(server
        server.cpp
        json.hpp
)

if(USE_OPENMP)
    if(APPLE)
        set(OpenMP_CXX_FLAGS "-Xpreprocessor -fopenmp")
        set(OpenMP_CXX_LIB_NAMES "omp")
        set(OpenMP_omp_LIBRARY "/opt/homebrew/opt/libomp/lib/libomp.dylib")
    endif()

    find_package(OpenMP COMPONENTS CXX)

    if(OpenMP_CXX_FOUND)
        target_link_libraries(client PRIVATE OpenMP::OpenMP_CXX)
        target_compile_definitions(client PRIVATE USE_OPENMP)
        message(STATUS "OpenMP enabled")
    else()
        message(WARNING "OpenMP requested but not found. Building without OpenMP.")
    endif()
endif()