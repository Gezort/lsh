//
// Created by Alexey Golomedov on 01.05.17.
//
#include "gtest/gtest.h"
#include <lsh.h>


class HammingHashTestCase: public ::testing::Test {
protected:
    void SetUp() {
        bstr.resize(2);
        bstr[0] = 4;
        bstr[1] = 3;
    }

    yasda::BinaryString bstr;
};

TEST_F(HammingHashTestCase, itShouldComputeHashCode) {
    yasda::HammingHash<yasda::BinaryString> hash(128, 65);
    EXPECT_TRUE(hash(bstr));

    yasda::HammingHash<yasda::BinaryString> hash_2(128, 8);
    EXPECT_FALSE(hash_2(bstr));

    yasda::HammingHash<yasda::BinaryString> hash_3(128, 512);
    EXPECT_FALSE(hash_3(bstr));
}
