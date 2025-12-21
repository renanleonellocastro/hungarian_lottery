#include <gtest/gtest.h>

#include "cached_lottery_processor.h"

using namespace hungarian;

TEST(CachedProcessorHelpers, GenerateCombinationsInvalidSizeThrows) {
    std::unique_ptr<ILotteryProcessor> p = std::make_unique<CachedLotteryProcessor>();
    ASSERT_TRUE(p);
    auto cached = dynamic_cast<CachedLotteryProcessor*>(p.get());
    ASSERT_NE(cached, nullptr);

    std::array<uint8_t,5> nums = {1,2,3,4,5};
    EXPECT_THROW(cached->test_generateCombinations(nums, 1), std::invalid_argument);
    EXPECT_THROW(cached->test_generateCombinations(nums, 6), std::invalid_argument);
}

TEST(CachedProcessorHelpers, PreProcessPlayerAndCountMatches) {
    std::unique_ptr<ILotteryProcessor> p = std::make_unique<CachedLotteryProcessor>();
    ASSERT_TRUE(p);
    auto cached = dynamic_cast<CachedLotteryProcessor*>(p.get());
    ASSERT_NE(cached, nullptr);

    std::array<uint8_t,5> pick = {5,4,3,2,1};
    // preProcessPlayer will sort and add entries
    cached->test_preProcessPlayer(pick);
    // After preprocessing, countMatches for the sorted combination should be >= 1
    std::array<uint8_t,5> sorted = pick;
    std::sort(sorted.begin(), sorted.end());
    uint32_t cnt = cached->test_countMatches(sorted);
    EXPECT_GE(cnt, 1u);
}
