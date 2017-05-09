//
// Created by Alexey Golomedov on 01.05.17.
//

#include "gtest/gtest.h"
#include <binarystring.h>

class SparseBinaryStringTest: public ::testing::Test {
protected:
    void SetUp() {
        bstr.insert(1);
        bstr.insert(5);
        bstr.insert(8);
    }

    yasda::SparseBinaryString bstr;
};

TEST_F(SparseBinaryStringTest, itShoulReadBits) {
    EXPECT_TRUE(yasda::GetBit(bstr, 1));
    EXPECT_FALSE(yasda::GetBit(bstr, 2));
    EXPECT_TRUE(yasda::GetBit(bstr, 8));
}

TEST_F(SparseBinaryStringTest, itShoudWriteBits) {
    yasda::SparseBinaryString bstrCpy = bstr;

    EXPECT_FALSE(yasda::GetBit(bstrCpy, 0));
    yasda::SetBit(bstrCpy, 0, true);
    EXPECT_TRUE(yasda::GetBit(bstrCpy, 0));

    EXPECT_TRUE(yasda::GetBit(bstrCpy, 8));
    yasda::SetBit(bstrCpy, 8, false);
    EXPECT_FALSE(yasda::GetBit(bstrCpy, 8));
}

TEST_F(SparseBinaryStringTest, itShouldComputeDistance) {
    yasda::SparseBinaryString bstr_2;
    bstr_2.insert(2);
    bstr_2.insert(5);
    bstr_2.insert(8);

    EXPECT_EQ(2, yasda::GetHammingDistance(bstr_2, bstr));
    EXPECT_EQ(0, yasda::GetHammingDistance(bstr, bstr));

    bstr_2.insert(1);
    EXPECT_EQ(1, yasda::GetHammingDistance(bstr_2, bstr));
}

TEST_F(SparseBinaryStringTest, itShouldCompareBinaryStrings) {
    yasda::SparseBinaryString bstr_2;
    bstr_2.insert(2);
    bstr_2.insert(5);
    bstr_2.insert(8);

    EXPECT_TRUE(yasda::BinaryStringsAreSame(bstr, bstr));
    EXPECT_FALSE(yasda::BinaryStringsAreSame(bstr, bstr_2));
}


TEST_F(SparseBinaryStringTest, itShouldCopyBinaryStrings) {
    yasda::SparseBinaryString bstr_2;
    bstr_2.insert(2);
    bstr_2.insert(5);
    bstr_2.insert(8);

    EXPECT_FALSE(yasda::BinaryStringsAreSame(bstr, bstr_2));
    yasda::CopyBinaryString(bstr, bstr_2);
    EXPECT_TRUE(yasda::BinaryStringsAreSame(bstr, bstr_2));
}
