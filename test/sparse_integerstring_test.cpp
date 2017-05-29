#include "gtest/gtest.h"
#include <integerstring.h>

class SparseIntegerStringTest: public ::testing::Test {
protected:
    void SetUp() {
        istr.insert({1, 10});
        istr.insert({5, 2});
        istr.insert({8, 5});
    }

    yasda::SparseIntegerString istr;
};

TEST_F(SparseIntegerStringTest, itShoulReadCoordinates) {
    EXPECT_EQ(yasda::GetCoordinate(istr, 1), 10);
    EXPECT_EQ(yasda::GetCoordinate(istr, 2), 0);
    EXPECT_EQ(yasda::GetCoordinate(istr, 8), 5);
}

TEST_F(SparseIntegerStringTest, itShoudWriteCoordinates) {
    yasda::SparseIntegerString istrCpy = istr;

    yasda::SetCoordinate(istrCpy, 0, 2);
    EXPECT_EQ(yasda::GetCoordinate(istrCpy, 0), 2);

    yasda::SetCoordinate(istrCpy, 8, 0);
    EXPECT_EQ(yasda::GetCoordinate(istrCpy, 8), 0);
}

TEST_F(SparseIntegerStringTest, itShouldComputeDistance) {
    yasda::SparseIntegerString istr_2;
    yasda::SetCoordinate(istr_2, 2, 1);
    yasda::SetCoordinate(istr_2, 5, 1);

    EXPECT_EQ(17, yasda::GetManhattanDistance(istr_2, istr));
    EXPECT_EQ(0, yasda::GetManhattanDistance(istr, istr));

    yasda::SetCoordinate(istr_2, 1, 10);
    EXPECT_EQ(7, yasda::GetManhattanDistance(istr_2, istr));
}

TEST_F(SparseIntegerStringTest, itShouldCompareIntegerStrings) {
    yasda::SparseIntegerString istr_2;
    yasda::SetCoordinate(istr_2, 1, 10);
    yasda::SetCoordinate(istr_2, 5, 2);
    yasda::SetCoordinate(istr_2, 8, 5);
    yasda::SetCoordinate(istr_2, 10, 0);

    EXPECT_TRUE(yasda::IntegerStringsAreSame(istr, istr));
    EXPECT_TRUE(yasda::IntegerStringsAreSame(istr, istr_2));

    yasda::SetCoordinate(istr_2, 10, 1);
    EXPECT_FALSE(yasda::IntegerStringsAreSame(istr, istr_2));
}


TEST_F(SparseIntegerStringTest, itShouldCopyIntegerStrings) {
    yasda::SparseIntegerString istr_2;
    yasda::SetCoordinate(istr_2, 1, 2);
    
    EXPECT_FALSE(yasda::IntegerStringsAreSame(istr, istr_2));
    yasda::CopyIntegerString(istr, istr_2);
    EXPECT_TRUE(yasda::IntegerStringsAreSame(istr, istr_2));
}
