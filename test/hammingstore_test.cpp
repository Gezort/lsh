//
// Created by Alexey Golomedov on 03.05.17.
//

#include "gtest/gtest.h"
#include <lsh.h>

class HammingStoreTest: public ::testing::Test {
protected:
    void SetUp() {
        bstr.resize(2);
        bstr[0] = ~0;
        bstr[1] = ~0;
    }

    yasda::BinaryString bstr;
};

TEST_F(HammingStoreTest, itShouldStoreAndFindElements) {

}

