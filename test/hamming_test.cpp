//
// Created by Alexey Golomedov on 01.05.17.
//

#include "gtest/gtest.h"
#include <hamming.h>

class HammingTestCase: public ::testing::Test {
protected:
    void SetUp() {
        bstr.resize(2);
        bstr[0] = 4;
        bstr[1] = 3;
    }

    yasda::BinaryString bstr;
};

TEST_F(HammingTestCase, itShoulReadBits) {
    EXPECT_FALSE(yasda::GetBit(bstr, 1));
    EXPECT_TRUE(yasda::GetBit(bstr, 2));
    EXPECT_TRUE(yasda::GetBit(bstr, 64));
}

TEST_F(HammingTestCase, itShoudWriteBits) {
    yasda::BinaryString bstrCpy = bstr;

    yasda::SetBit(bstrCpy, 0, true);
    EXPECT_TRUE(yasda::GetBit(bstrCpy, 0));
    EXPECT_EQ(bstrCpy[0], 5);

    yasda::SetBit(bstrCpy, 65, false);
    EXPECT_FALSE(yasda::GetBit(bstrCpy, 65));
    EXPECT_EQ(bstrCpy[1], 1);
}

