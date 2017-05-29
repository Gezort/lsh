#include "gtest/gtest.h"
#include <integerstring.h>

class HashedSparseIntegerStringTest: public ::testing::Test {
protected:
    void SetUp() {
        istr = new yasda::HashedSparseIntegerString(3);
        istr->triggeredCoordinates_.insert({1, 1});
        istr->triggeredCoordinates_.insert({5, 5});
        istr->triggeredCoordinates_.insert({10, 10});
    }

    void TearDown() {
        delete istr;
    }

    yasda::HashedSparseIntegerString* istr;
};

TEST_F(HashedSparseIntegerStringTest, itShoulReadCoordinates) {
    EXPECT_EQ(yasda::GetCoordinate(*istr, 1), 1);
    EXPECT_EQ(yasda::GetCoordinate(*istr, 2), 0);
    EXPECT_EQ(yasda::GetCoordinate(*istr, 10), 0);
}

TEST_F(HashedSparseIntegerStringTest, itShoudWriteCoordinates) {
    EXPECT_EQ(yasda::GetCoordinate(*istr, 10), 0);
    EXPECT_EQ(yasda::GetCoordinate(*istr, 2), 0);
    yasda::SetCoordinate(*istr, 10, 7);
    EXPECT_EQ(yasda::GetCoordinate(*istr, 10), 7);
    EXPECT_EQ(yasda::GetCoordinate(*istr, 2), 7);

    yasda::SetCoordinate(*istr, 10, 0);
    EXPECT_EQ(yasda::GetCoordinate(*istr, 10), 0);
    EXPECT_EQ(yasda::GetCoordinate(*istr, 2), 0);
}

TEST_F(HashedSparseIntegerStringTest, itShouldComputeDistance) {
    yasda::HashedSparseIntegerString istr_2(3);
    istr_2.triggeredCoordinates_.insert({2, 2});
    istr_2.triggeredCoordinates_.insert({5, 5});
    istr_2.triggeredCoordinates_.insert({8, 8});

    EXPECT_EQ(21, yasda::GetManhattanDistance(istr_2, *istr));
    EXPECT_EQ(0, yasda::GetManhattanDistance(*istr, *istr));

    istr_2.triggeredCoordinates_.insert({10, 10});
    EXPECT_EQ(11, yasda::GetManhattanDistance(istr_2, *istr));
}

TEST_F(HashedSparseIntegerStringTest, itShouldCompareIntegerStrings) {
    yasda::HashedSparseIntegerString istr_2(3);
    istr_2.triggeredCoordinates_.insert({2, 2});
    istr_2.triggeredCoordinates_.insert({5, 2});
    istr_2.triggeredCoordinates_.insert({8, 2});

    EXPECT_TRUE(yasda::IntegerStringsAreSame(*istr, *istr));
    EXPECT_FALSE(yasda::IntegerStringsAreSame(*istr, istr_2));
}


TEST_F(HashedSparseIntegerStringTest, itShouldCopyIntegerStrings) {
    yasda::HashedSparseIntegerString istr_2(3);
    istr_2.triggeredCoordinates_.insert({2, 2});
    istr_2.triggeredCoordinates_.insert({5, 5});
    istr_2.triggeredCoordinates_.insert({8, 8});

    EXPECT_FALSE(yasda::IntegerStringsAreSame(*istr, istr_2));
    yasda::CopyIntegerString(*istr, istr_2);
    EXPECT_TRUE(yasda::IntegerStringsAreSame(*istr, istr_2));
}
