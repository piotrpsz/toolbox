cmake_minimum_required(VERSION 3.31)
project(toolbox)

set(CMAKE_CXX_STANDARD 23)

find_package(date REQUIRED)

add_library(toolbox STATIC
        toolbox.h toolbox.cpp
        datime.h
        all.h
)

target_include_directories(toolbox PUBLIC date)
target_link_libraries(toolbox PUBLIC date::date date::date-tz)
