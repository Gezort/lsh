#include "gtest/gtest.h"
#include <integerstring.h>

class DenseIntegerStringTest: public ::testing::Test {
protected:
    void SetUp() {
        istr.resize(2);
        istr[0] = 4;
        istr[1] = 3;
    }

    yasda::IntegerString istr;
};

TEST_F(DenseIntegerStringTest, itShouldReadCoodinates) {
    EXPECT_EQ(yasda::GetCoordinate(istr, 0), 4);
    EXPECT_EQ(yasda::GetCoordinate(istr, 1), 3);
    EXPECT_EQ(yasda::GetCoordinate(istr, 5), 0);
}

TEST_F(DenseIntegerStringTest, itShouldWriteCoodinates) {
    yasda::IntegerString istrCpy = istr;

    yasda::SetCoordinate(istrCpy, 3, 20);
    EXPECT_EQ(yasda::GetCoordinate(istrCpy, 3), 20);

    yasda::SetCoordinate(istrCpy, 1, 0);
    EXPECT_EQ(yasda::GetCoordinate(istrCpy, 1), 0);
}

TEST_F(DenseIntegerStringTest, itShouldComputeDistance) {
    yasda::IntegerString istr_2(2);
    istr_2[0] = 5;
    istr_2[1] = 1;

    EXPECT_EQ(3, yasda::GetManhattanDistance(istr_2, istr));

}

TEST_F(DenseIntegerStringTest, itShouldCompareIntegerStrings) {
    yasda::IntegerString istr_2(2);
    istr_2[0] = 5;
    istr_2[1] = 2;

    EXPECT_TRUE(yasda::IntegerStringsAreSame(istr, istr));
    EXPECT_FALSE(yasda::IntegerStringsAreSame(istr, istr_2));
}

TEST_F(DenseIntegerStringTest, itShouldCopyIntegerStrings) {
    yasda::IntegerString istr_2(2);
    istr_2[0] = 5;
    istr_2[1] = 2;

    EXPECT_FALSE(yasda::IntegerStringsAreSame(istr, istr_2));
    yasda::CopyIntegerString(istr, istr_2);
    EXPECT_TRUE(yasda::IntegerStringsAreSame(istr, istr_2));
}
