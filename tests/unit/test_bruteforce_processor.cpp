#include <gtest/gtest.h>

#include "brute_force_lottery_processor.h"

using namespace hungarian;

TEST(BruteForceProcessor, ComputeWinners) {
    BruteForceLotteryProcessor p;
    std::vector<std::vector<uint8_t>> players = {
        {1,2,3,4,5}, // five
        {1,2,3,4,6}, // four
        {1,2,3,7,8}, // three
        {1,2,9,10,11}, // two
        {12,13,14,15,16} // zero
    };
    p.preComputeWinners(players);

    std::array<uint8_t,5> pick = {1,2,3,4,5};
    Winners w = p.computeWinners(pick);
    EXPECT_EQ(w.five, 1u);
    EXPECT_EQ(w.four, 1u);
    EXPECT_EQ(w.three, 1u);
    EXPECT_EQ(w.two, 1u);
}
