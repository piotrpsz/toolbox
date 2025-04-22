cmake_minimum_required(VERSION 3.31)
project(tool)
set(CMAKE_CXX_STANDARD 23)

find_package(date REQUIRED)

add_library(tool STATIC
        toolbox.cpp
        datime.h
        types.h
        lzav.h
        file.h
        all.hpp
        crypto/crypto.cpp
        crypto/crypto.h
        crypto/blowfish/blowfish.cpp
        crypto/blowfish/blowfish.h
        crypto/gost/gost.cpp
        crypto/gost/gost.h
)

target_include_directories(tool PUBLIC date)
target_link_libraries(tool PUBLIC date::date date::date-tz)