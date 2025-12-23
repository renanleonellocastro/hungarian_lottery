#include <gtest/gtest.h>
#include <array>
#include <vector>

#include "cached_lottery_processor.h"

using namespace hungarian;

TEST(CachedProcessorTargeted, GenerateCombinationsAndEncodeKey) {
    std::unique_ptr<ILotteryProcessor> cachedLotteryProcessor = std::make_unique<CachedLotteryProcessor>();
    ASSERT_TRUE(cachedLotteryProcessor);
    auto cached = dynamic_cast<CachedLotteryProcessor*>(cachedLotteryProcessor.get());
    ASSERT_NE(cached, nullptr);
    std::array<uint8_t,5> numbers{1,2,3,4,5};

    // Valid combination sizes
    auto c2 = cached->test_generateCombinations(numbers, 2);
    auto c3 = cached->test_generateCombinations(numbers, 3);
    auto c4 = cached->test_generateCombinations(numbers, 4);

    EXPECT_EQ(c2.size(), 10);
    EXPECT_EQ(c3.size(), 10);
    EXPECT_EQ(c4.size(), 5);

    // Invalid sizes throw
    EXPECT_THROW(cached->test_generateCombinations(numbers, 1), std::invalid_argument);
    EXPECT_THROW(cached->test_generateCombinations(numbers, 5), std::invalid_argument);

    // getHashKeyFromNumbersCombination (test wrapper) should normalize (sort) input
    std::array<uint8_t,5> unsorted{5,1,3,2,4};
    auto key1 = cached->test_encodeKey(numbers);
    auto key2 = cached->test_getHashKeyFromNumbersCombination(unsorted);
    EXPECT_EQ(key1, key2);
}

TEST(CachedProcessorTargeted, PreProcessAndCountMatchesComputeWinners) {

    std::unique_ptr<ILotteryProcessor> cachedLotteryProcessor = std::make_unique<CachedLotteryProcessor>();
    ASSERT_TRUE(cachedLotteryProcessor);
    auto cached = dynamic_cast<CachedLotteryProcessor*>(cachedLotteryProcessor.get());
    ASSERT_NE(cached, nullptr);

    // initialize cache
    cached->preComputeWinners({});
    std::array<uint8_t,5> pick{1,2,3,4,5};

    // Initially zero
    EXPECT_EQ(cached->test_countMatches(pick), 0);
    // Add a player with exact same pick
    cached->test_preProcessPlayer(pick);

    // The full pick must be counted
    EXPECT_EQ(cached->test_countMatches(pick), 1);

    // computeWinners should report one 5-match and zero for others
    Winners w = cached->computeWinners(pick);
    EXPECT_EQ(w.five, 1);
    EXPECT_EQ(w.four, 0);
    EXPECT_EQ(w.three, 0);
    EXPECT_EQ(w.two, 0);
}
