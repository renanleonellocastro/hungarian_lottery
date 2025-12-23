#include <gtest/gtest.h>

#include "brute_force_lottery_processor.h"

using namespace hungarian;

TEST(BruteForceProcessor, ComputeWinners) {
    BruteForceLotteryProcessor bruteForceLotteryProcessor;
    std::vector<std::array<uint8_t,5>> players = {
        std::array<uint8_t,5>{1,2,3,4,5}, // five
        std::array<uint8_t,5>{1,2,3,4,6}, // four
        std::array<uint8_t,5>{1,2,3,7,8}, // three
        std::array<uint8_t,5>{1,2,9,10,11}, // two
        std::array<uint8_t,5>{12,13,14,15,16} // zero
    };

    bruteForceLotteryProcessor.preComputeWinners(players);

    std::array<uint8_t,5> pick = {1,2,3,4,5};
    Winners w = bruteForceLotteryProcessor.computeWinners(pick);
    EXPECT_EQ(w.five, 1);
    EXPECT_EQ(w.four, 1);
    EXPECT_EQ(w.three, 1);
    EXPECT_EQ(w.two, 1);
}
