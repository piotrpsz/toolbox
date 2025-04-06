cmake_minimum_required(VERSION 3.30)
project(toolbox)

set(CMAKE_CXX_STANDARD 23)

find_package(date REQUIRED)
add_subdirectory(tests)

add_library(toolbox STATIC
        toolbox.cpp toolbox.h
        datime.h
        all.h
        crypto/blowfish/data.h
        types.h
        crypto/blowfish/blowfish.cpp
        crypto/blowfish/blowfish.h
        crypto/crypto.cpp
        crypto/crypto.h
        crypto/gost/gost.cpp
        crypto/gost/gost.h
        file.h
        lzav.h
        tests/main.cc
        tests/toolbox_test.cc
)

target_include_directories(toolbox PUBLIC date)
target_link_libraries(toolbox PUBLIC date::date date::date-tz)
