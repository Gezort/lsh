//
// Created by Alexey Golomedov on 01.05.17.
//

#include "gtest/gtest.h"
#include <lsh.h>

class HashGroupTest: public ::testing::Test {
protected:
    void SetUp() {
        bstr.resize(2);
        bstr[0] = ~0;
        bstr[1] = ~0;
    }

    yasda::BinaryString bstr;
};

TEST_F(HashGroupTest, itShouldConstructHashGroup) {
    std::random_device rd;
    yasda::HammingHashFamily<yasda::BinaryString> hashFamily(128, 0, 0);
    yasda::HashGroup<yasda::BinaryString, yasda::HammingHashFamily, 
                    yasda::HammingHash> hashGroup(hashFamily, 32, rd);

    EXPECT_TRUE(hashGroup(bstr) > 0);
}

