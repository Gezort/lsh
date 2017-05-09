//
// Created by Alexey Golomedov on 08.05.17.
//

#include "gtest/gtest.h"
#include <binarystring.h>

TEST(HashedBinaryStringReaderTest, itShouldReadFiles) {
    yasda::HashedBinaryStringReader reader("./sample_in_file", 16);
    const std::vector<yasda::HashedSparseBinaryString*> X = reader.get();

    EXPECT_EQ(2, X.size());

    EXPECT_TRUE(yasda::GetBit(*X[0], 42));
    EXPECT_TRUE(yasda::GetBit(*X[0], 8));
    EXPECT_TRUE(yasda::GetBit(*X[0], 12));

    EXPECT_TRUE(yasda::GetBit(*X[1], 56));
    EXPECT_TRUE(yasda::GetBit(*X[1], 13));
    EXPECT_TRUE(yasda::GetBit(*X[1], 19));
    EXPECT_TRUE(yasda::GetBit(*X[1], 2));

    EXPECT_FALSE(yasda::GetBit(*X[0], 2));
    EXPECT_FALSE(yasda::GetBit(*X[1], 42));
    EXPECT_FALSE(yasda::GetBit(*X[0], 0));
    EXPECT_FALSE(yasda::GetBit(*X[1], 1));
}

TEST(HashedBinaryStringReaderTest, itShouldStoreId) {
    yasda::HashedBinaryStringReader reader("./sample_in_file", 16);
    const std::vector<yasda::HashedSparseBinaryString*> X = reader.get();

    EXPECT_EQ(0, reader.getId(reader.get()[0]));
    EXPECT_EQ(1, reader.getId(reader.get()[1]));
}