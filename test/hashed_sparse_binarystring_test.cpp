//
// Created by Alexey Golomedov on 01.05.17.
//

#include "gtest/gtest.h"
#include <binarystring.h>

class HashedSparseBinaryStringTest: public ::testing::Test {
protected:
    void SetUp() {
        bstr = new yasda::HashedSparseBinaryString(3);
        bstr->triggeredBits_.insert(1);
        bstr->triggeredBits_.insert(5);
        bstr->triggeredBits_.insert(10);
    }

    void TearDown() {
        delete bstr;
    }

    yasda::HashedSparseBinaryString* bstr;
};

TEST_F(HashedSparseBinaryStringTest, itShoulReadBits) {
    EXPECT_TRUE(yasda::GetBit(*bstr, 1));
    EXPECT_FALSE(yasda::GetBit(*bstr, 2));
    EXPECT_FALSE(yasda::GetBit(*bstr, 10));
}

TEST_F(HashedSparseBinaryStringTest, itShoudWriteBits) {
    EXPECT_FALSE(yasda::GetBit(*bstr, 10));
    EXPECT_FALSE(yasda::GetBit(*bstr, 2));
    yasda::SetBit(*bstr, 10, true);
    EXPECT_TRUE(yasda::GetBit(*bstr, 10));
    EXPECT_TRUE(yasda::GetBit(*bstr, 2));

    yasda::SetBit(*bstr, 10, false);
    EXPECT_FALSE(yasda::GetBit(*bstr, 10));
    EXPECT_FALSE(yasda::GetBit(*bstr, 2));
}

TEST_F(HashedSparseBinaryStringTest, itShouldComputeDistance) {
    yasda::HashedSparseBinaryString bstr_2(3);
    bstr_2.triggeredBits_.insert(2);
    bstr_2.triggeredBits_.insert(5);
    bstr_2.triggeredBits_.insert(8);

    EXPECT_EQ(4, yasda::GetHammingDistance(bstr_2, *bstr));
    EXPECT_EQ(0, yasda::GetHammingDistance(*bstr, *bstr));

    bstr_2.triggeredBits_.insert(1);
    EXPECT_EQ(3, yasda::GetHammingDistance(bstr_2, *bstr));
}

TEST_F(HashedSparseBinaryStringTest, itShouldCompareBinaryStrings) {
    yasda::HashedSparseBinaryString bstr_2(3);
    bstr_2.triggeredBits_.insert(2);
    bstr_2.triggeredBits_.insert(5);
    bstr_2.triggeredBits_.insert(8);

    EXPECT_TRUE(yasda::BinaryStringsAreSame(*bstr, *bstr));
    EXPECT_FALSE(yasda::BinaryStringsAreSame(*bstr, bstr_2));
}


TEST_F(HashedSparseBinaryStringTest, itShouldCopyBinaryStrings) {
    yasda::HashedSparseBinaryString bstr_2(3);
    bstr_2.triggeredBits_.insert(2);
    bstr_2.triggeredBits_.insert(5);
    bstr_2.triggeredBits_.insert(8);

    EXPECT_FALSE(yasda::BinaryStringsAreSame(*bstr, bstr_2));
    yasda::CopyBinaryString(*bstr, bstr_2);
    EXPECT_TRUE(yasda::BinaryStringsAreSame(*bstr, bstr_2));
}
