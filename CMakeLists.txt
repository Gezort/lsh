cmake_minimum_required(VERSION 3.8)
project(lsh)

set(CMAKE_CXX_STANDARD 11)

find_package(Boost 1.63 COMPONENTS program_options REQUIRED)

include_directories(include ${Boost_INCLUDE_DIR})

add_library(
        lsh
        src/dense_binarystring.cpp src/sparse_binarystring.cpp src/hashed_sparse_binarystring.cpp src/binarystring.cpp
        src/dense_integerstring.cpp src/integerstring.cpp src/sparse_integerstring.cpp src/hashed_sparse_integerstring.cpp
)
target_link_libraries(lsh)

enable_testing()
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIR})

add_executable(dense_integerstring_test test/dense_integerstring_test.cpp)
target_link_libraries(dense_integerstring_test lsh gtest gtest_main)
add_test(dense_integerstring_test dense_integerstring_test)

add_executable(dense_binarystring_test test/dense_binarystring_test.cpp)
target_link_libraries(dense_binarystring_test lsh gtest gtest_main)
add_test(dense_binarystring_test dense_binarystring_test)

add_executable(hamminghash_test test/hamminghash_test.cpp)
target_link_libraries(hamminghash_test lsh gtest gtest_main)
add_test(hamminghash_test hamminghash_test)

add_executable(hashgroup_test test/hashgroup_test.cpp)
target_link_libraries(hashgroup_test lsh gtest gtest_main)
add_test(hashgroup_test hashgroup_test)

add_executable(rnn_test test/approximate_rnn_test.cpp)
target_link_libraries(rnn_test lsh gtest gtest_main)
add_test(rnn_test rnn_test)

add_executable(sparse_binarystring_test test/sparse_binarystring_test.cpp)
target_link_libraries(sparse_binarystring_test lsh gtest gtest_main)
add_test(sparse_binarystring_test sparse_binarystring_test)

add_executable(sparse_integerstring_test test/sparse_integerstring_test.cpp)
target_link_libraries(sparse_integerstring_test lsh gtest gtest_main)
add_test(sparse_integerstring_test sparse_integerstring_test)

add_executable(hashed_sparse_binarystring_test test/hashed_sparse_binarystring_test.cpp)
target_link_libraries(hashed_sparse_binarystring_test lsh gtest gtest_main)
add_test(hashed_sparse_binarystring_test hashed_sparse_binarystring_test)

add_executable(hashed_sparse_integerstring_test test/hashed_sparse_integerstring_test.cpp)
target_link_libraries(hashed_sparse_integerstring_test lsh gtest gtest_main)
add_test(hashed_sparse_integerstring_test hashed_sparse_integerstring_test)

add_executable(hashed_bstr_reader_test test/hashed_bstr_reader_test.cpp)
target_link_libraries(hashed_bstr_reader_test lsh gtest gtest_main)
add_test(hashed_bstr_reader_test hashed_bstr_reader_test)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test/sample_in_file ${CMAKE_CURRENT_BINARY_DIR}/sample_in_file COPYONLY)

add_executable(krnn src/krnn.cpp)
target_link_libraries(krnn lsh ${Boost_PROGRAM_OPTIONS_LIBRARY})
