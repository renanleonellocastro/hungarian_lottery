#include <gtest/gtest.h>

#include "cached_lottery_processor.h"

using namespace hungarian;

TEST(CachedProcessorExtra, EmptyPlayersProducesZeros) {
    std::unique_ptr<ILotteryProcessor> cached = std::make_unique<CachedLotteryProcessor>();
    ASSERT_TRUE(cached);
    std::vector<std::vector<uint8_t>> players; // empty
    cached->preComputeWinners(players);
    std::array<uint8_t,5> pick = {1,2,3,4,5};
    Winners w = cached->computeWinners(pick);
    EXPECT_EQ(w.five, 0u);
    EXPECT_EQ(w.four, 0u);
    EXPECT_EQ(w.three, 0u);
    EXPECT_EQ(w.two, 0u);
}

TEST(CachedProcessorExtra, CountsMultipleMatchesAndDuplicates) {
    // Build players that create 1 five-match, 2 four-matches, 1 three-match, 1 two-match
    std::vector<std::vector<uint8_t>> players = {
        {1,2,3,4,5},   // five
        {1,2,3,4,6},   // four
        {1,2,3,4,6},   // four duplicate (should increment four count)
        {1,2,3,7,8},   // three
        {1,2,9,10,11}  // two
    };

    std::unique_ptr<ILotteryProcessor> cached = std::make_unique<CachedLotteryProcessor>();
    ASSERT_TRUE(cached);
    cached->preComputeWinners(players);

    std::array<uint8_t,5> pick = {1,2,3,4,5};
    Winners w = cached->computeWinners(pick);

    EXPECT_EQ(w.five, 1u);
    EXPECT_EQ(w.four, 2u);
    EXPECT_EQ(w.three, 1u);
    EXPECT_EQ(w.two, 1u);
}

TEST(CachedProcessorExtra, AllSameCombinationCountsAccumulate) {
    // 10 players with same exact numbers -> should count as 10 five-matches
    std::vector<std::vector<uint8_t>> players;
    for (int i=0;i<10;++i) players.push_back({5,4,3,2,1});

    std::unique_ptr<ILotteryProcessor> cached = std::make_unique<CachedLotteryProcessor>();
    ASSERT_TRUE(cached);
    cached->preComputeWinners(players);

    std::array<uint8_t,5> pick = {1,2,3,4,5};
    Winners w = cached->computeWinners(pick);

    EXPECT_EQ(w.five, 10u);
    EXPECT_EQ(w.four, 0u);
    EXPECT_EQ(w.three, 0u);
    EXPECT_EQ(w.two, 0u);
}
