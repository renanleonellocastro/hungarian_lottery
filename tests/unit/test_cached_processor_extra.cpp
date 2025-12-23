#include <gtest/gtest.h>

#include "cached_lottery_processor.h"

using namespace hungarian;

TEST(CachedProcessorExtra, EmptyPlayersProducesZeros) {
    std::unique_ptr<ILotteryProcessor> cached = std::make_unique<CachedLotteryProcessor>();
    ASSERT_TRUE(cached);
    std::vector<std::array<uint8_t,5>> players; // empty
    cached->preComputeWinners(players);
    std::array<uint8_t,5> pick = {1,2,3,4,5};
    Winners w = cached->computeWinners(pick);
    EXPECT_EQ(w.five, 0);
    EXPECT_EQ(w.four, 0);
    EXPECT_EQ(w.three, 0);
    EXPECT_EQ(w.two, 0);
}

TEST(CachedProcessorExtra, CountsMultipleMatchesAndDuplicates) {
    // Build players that create 1 five-match, 2 four-matches, 1 three-match, 1 two-match
    std::vector<std::array<uint8_t,5>> players = {
        std::array<uint8_t,5>{1,2,3,4,5},   // five
        std::array<uint8_t,5>{1,2,3,4,6},   // four
        std::array<uint8_t,5>{1,2,3,4,6},   // four duplicate (should increment four count)
        std::array<uint8_t,5>{1,2,3,7,8},   // three
        std::array<uint8_t,5>{1,2,9,10,11}  // two
    };

    std::unique_ptr<ILotteryProcessor> cached = std::make_unique<CachedLotteryProcessor>();
    ASSERT_TRUE(cached);
    cached->preComputeWinners(players);

    std::array<uint8_t,5> pick = {1,2,3,4,5};
    Winners w = cached->computeWinners(pick);

    EXPECT_EQ(w.five, 1);
    EXPECT_EQ(w.four, 2);
    EXPECT_EQ(w.three, 1);
    EXPECT_EQ(w.two, 1);
}

TEST(CachedProcessorExtra, AllSameCombinationCountsAccumulate) {
    // 10 players with same exact numbers -> should count as 10 five-matches
    std::vector<std::array<uint8_t,5>> players;

    for (int i=0;i<10;++i) {
        players.push_back(std::array<uint8_t,5>{5,4,3,2,1});
    }

    std::unique_ptr<ILotteryProcessor> cached = std::make_unique<CachedLotteryProcessor>();
    ASSERT_TRUE(cached);
    cached->preComputeWinners(players);

    std::array<uint8_t,5> pick = {1,2,3,4,5};
    Winners w = cached->computeWinners(pick);

    EXPECT_EQ(w.five, 10);
    EXPECT_EQ(w.four, 0);
    EXPECT_EQ(w.three, 0);
    EXPECT_EQ(w.two, 0);
}
