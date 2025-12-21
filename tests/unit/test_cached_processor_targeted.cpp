#include <gtest/gtest.h>
#include <array>
#include <vector>

#include "cached_lottery_processor.h"

using namespace hungarian;

TEST(CachedProcessorTargeted, GenerateCombinationsAndEncodeKey) {
    std::unique_ptr<ILotteryProcessor> p = std::make_unique<CachedLotteryProcessor>();
    ASSERT_TRUE(p);
    auto proc = dynamic_cast<CachedLotteryProcessor*>(p.get());
    ASSERT_NE(proc, nullptr);
    std::array<uint8_t,5> numbers{1,2,3,4,5};

    // Valid combination sizes
    auto c2 = proc->test_generateCombinations(numbers, 2);
    auto c3 = proc->test_generateCombinations(numbers, 3);
    auto c4 = proc->test_generateCombinations(numbers, 4);

    EXPECT_EQ(c2.size(), 10u);
    EXPECT_EQ(c3.size(), 10u);
    EXPECT_EQ(c4.size(), 5u);

    // Invalid sizes throw
    EXPECT_THROW(proc->test_generateCombinations(numbers, 1), std::invalid_argument);
    EXPECT_THROW(proc->test_generateCombinations(numbers, 5), std::invalid_argument);

    // getHashKeyFromNumbersCombination (test wrapper) should normalize (sort) input
    std::array<uint8_t,5> unsorted{5,1,3,2,4};
    auto key1 = proc->test_encodeKey(numbers);
    auto key2 = proc->test_getHashKeyFromNumbersCombination(unsorted);
    EXPECT_EQ(key1, key2);
}

TEST(CachedProcessorTargeted, PreProcessAndCountMatchesComputeWinners) {

    std::unique_ptr<ILotteryProcessor> p2 = std::make_unique<CachedLotteryProcessor>();
    ASSERT_TRUE(p2);
    auto proc2 = dynamic_cast<CachedLotteryProcessor*>(p2.get());
    ASSERT_NE(proc2, nullptr);

    // initialize cache
    proc2->preComputeWinners({});

    std::array<uint8_t,5> pick{1,2,3,4,5};

    // Initially zero
    EXPECT_EQ(proc2->test_countMatches(pick), 0u);

    // Add a player with exact same pick
    proc2->test_preProcessPlayer(pick);

    // The full pick must be counted
    EXPECT_EQ(proc2->test_countMatches(pick), 1u);

    // computeWinners should report one 5-match and zero for others
    Winners w = proc2->computeWinners(pick);
    EXPECT_EQ(w.five, 1u);
    EXPECT_EQ(w.four, 0u);
    EXPECT_EQ(w.three, 0u);
    EXPECT_EQ(w.two, 0u);
}
