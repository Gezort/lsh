//
// Created by Alexey Golomedov on 04.05.17.
//

//
// Created by Alexey Golomedov on 01.05.17.
//

#include "gtest/gtest.h"
#include <hamming.h>
#include <lsh.h>
#include <random>

class ApproximateRNNTestCase: public ::testing::Test {
protected:
    void SetUp() {
        std::random_device rd;
        for (size_t bstrId=0; bstrId < N; ++bstrId) {
            X.push_back(generateRandomString(d, rd));
        }
    }

    yasda::BinaryString* generateRandomString(size_t d, std::random_device& rd) {
        size_t groups = d / groupSize;
        std::mt19937 mt(rd());
        std::uniform_int_distribution<int> uniformDistribution(0, 1);

        yasda::BinaryString* bstr = new yasda::BinaryString(groups);

        size_t bitId = 0;
        for (size_t groupId = 0; groupId < groups; ++groupId) {
            size_t bitInGroup = 0;

            while (bitId < d && bitInGroup < groupSize) {
                yasda::SetBit(*bstr, bitId, (uniformDistribution(mt) == 1));

                ++bitId;
                ++bitInGroup;
            }
        }

        return bstr;
    }

    void TearDown() {
        for (auto x: X) {
            delete x;
        }
    }

    std::vector<yasda::BinaryString*> X;

    size_t d = 32;
    size_t N = 512;

    const size_t groupSize = 64;
};

TEST_F(ApproximateRNNTestCase, itShouldBeAbleToConstruct) {
    yasda::ApproximateRNN rnn(N, 2, 0.5, 128, 2, -1, 1, -1, 16, 32);
    EXPECT_EQ(1, rnn.getLSHStoresCount());
}

TEST_F(ApproximateRNNTestCase, itShouldFindElementItself) {
    yasda::ApproximateRNN rnn(N, 2, 0.5, 128, 2, -1, 1, -1, 16, 32);

    rnn.fit(X);
    std::vector<yasda::BinaryString*> neighbours = rnn.getKNearestNeighbours(*X[0], 1);

    EXPECT_EQ(1, neighbours.size());
    EXPECT_EQ(X[0], neighbours[0]);
}

