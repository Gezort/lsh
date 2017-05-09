//
// Created by Alexey Golomedov on 01.05.17.
//

#include "gtest/gtest.h"
#include <binarystring.h>

class DenseBinaryStringTest: public ::testing::Test {
protected:
    void SetUp() {
        bstr.resize(2);
        bstr[0] = 4;
        bstr[1] = 3;
    }

    yasda::BinaryString bstr;
};

TEST_F(DenseBinaryStringTest, itShoulReadBits) {
    EXPECT_FALSE(yasda::GetBit(bstr, 1));
    EXPECT_TRUE(yasda::GetBit(bstr, 2));
    EXPECT_TRUE(yasda::GetBit(bstr, 64));
}

TEST_F(DenseBinaryStringTest, itShoudWriteBits) {
    yasda::BinaryString bstrCpy = bstr;

    yasda::SetBit(bstrCpy, 0, true);
    EXPECT_TRUE(yasda::GetBit(bstrCpy, 0));
    EXPECT_EQ(bstrCpy[0], 5);

    yasda::SetBit(bstrCpy, 65, false);
    EXPECT_FALSE(yasda::GetBit(bstrCpy, 65));
    EXPECT_EQ(bstrCpy[1], 1);
}

TEST_F(DenseBinaryStringTest, itShouldComputeDistance) {
    yasda::BinaryString bstr_2(2);
    bstr_2[0] = 5;
    bstr_2[1] = 2;

    EXPECT_EQ(2, yasda::GetHammingDistance(bstr_2, bstr));

}

TEST_F(DenseBinaryStringTest, itShouldCompareBinaryStrings) {
    yasda::BinaryString bstr_2(2);
    bstr_2[0] = 5;
    bstr_2[1] = 2;

    EXPECT_TRUE(yasda::BinaryStringsAreSame(bstr, bstr));
    EXPECT_FALSE(yasda::BinaryStringsAreSame(bstr, bstr_2));
}

TEST_F(DenseBinaryStringTest, itShouldCopyBinaryStrings) {
    yasda::BinaryString bstr_2(2);
    bstr_2[0] = 5;
    bstr_2[1] = 2;

    EXPECT_FALSE(yasda::BinaryStringsAreSame(bstr, bstr_2));
    yasda::CopyBinaryString(bstr, bstr_2);
    EXPECT_TRUE(yasda::BinaryStringsAreSame(bstr, bstr_2));
}
