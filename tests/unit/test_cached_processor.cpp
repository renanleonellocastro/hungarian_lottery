#include <gtest/gtest.h>

#include "cached_lottery_processor.h"
#include "brute_force_lottery_processor.h"

using namespace hungarian;

TEST(CachedProcessor, MatchesBruteForce) {
    BruteForceLotteryProcessor bruteForceLotteryProcessor;
    std::vector<std::array<uint8_t,5>> players = {
        std::array<uint8_t,5>{1,2,3,4,5},
        std::array<uint8_t,5>{1,2,3,4,6},
        std::array<uint8_t,5>{1,2,3,7,8},
        std::array<uint8_t,5>{1,2,9,10,11},
        std::array<uint8_t,5>{12,13,14,15,16}
    };
    bruteForceLotteryProcessor.preComputeWinners(players);
    std::array<uint8_t,5> pick = {1,2,3,4,5};
    Winners wbf = bruteForceLotteryProcessor.computeWinners(pick);

    std::unique_ptr<ILotteryProcessor> cached = std::make_unique<CachedLotteryProcessor>();
    ASSERT_TRUE(cached);
    cached->preComputeWinners(players);
    Winners wc = cached->computeWinners(pick);

    EXPECT_EQ(wc.five, wbf.five);
    EXPECT_EQ(wc.four, wbf.four);
    EXPECT_EQ(wc.three, wbf.three);
    EXPECT_EQ(wc.two, wbf.two);
}
