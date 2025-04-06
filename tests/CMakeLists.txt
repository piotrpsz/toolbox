cmake_minimum_required(VERSION 3.30)
message("OS: ${CMAKE_SYSTEM_NAME}")

find_package(GTest REQUIRED)
enable_testing()

add_executable(test_app
        main.cc
        toolbox_test.cc
        ../toolbox.cpp ../toolbox.h
)

target_link_libraries(test_app PUBLIC
        GTest::gtest_main
        GTest::gtest
)

target_include_directories(test_app PUBLIC
        GTest
)
include(GoogleTest)
gtest_discover_tests(test_app)
