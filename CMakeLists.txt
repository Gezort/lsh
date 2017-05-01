cmake_minimum_required(VERSION 3.8)
project(lsh)

set(CMAKE_CXX_STANDARD 11)


include_directories(include)

add_library(lsh src/hamming.cpp src/lsh.cpp)
target_link_libraries(lsh)

enable_testing()
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIR})

add_executable(hamming_test test/hamming_test.cpp)
target_link_libraries(hamming_test lsh gtest gtest_main)
add_test(hamming_test hamming_test)

add_executable(hamminghash_test test/hamminghash_test.cpp)
target_link_libraries(hamminghash_test lsh gtest gtest_main)
add_test(hamminghash_test hamminghash_test)