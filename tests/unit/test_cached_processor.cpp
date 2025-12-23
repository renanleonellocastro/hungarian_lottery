#include <gtest/gtest.h>

#include <array>
#include <vector>
#include <memory>
#include <algorithm>

#include "cached_lottery_processor.h"
#include "brute_force_lottery_processor.h"

using namespace hungarian;

static std::array<uint8_t,5> pick(uint8_t a,uint8_t b,uint8_t c,uint8_t d,uint8_t e) {
    return {a,b,c,d,e};
}

TEST(CachedLotteryProcessor, MatchesBruteForceImplementation) {
    BruteForceLotteryProcessor brute;
    CachedLotteryProcessor cached;

    std::vector<std::array<uint8_t,5>> players = {
        pick(1,2,3,4,5),
        pick(1,2,3,4,6),
        pick(1,2,3,7,8),
        pick(1,2,9,10,11),
        pick(12,13,14,15,16)
    };

    brute.preComputeWinners(players);
    cached.preComputeWinners(players);

    auto lotteryPick = pick(1,2,3,4,5);

    Winners bw = brute.computeWinners(lotteryPick);
    Winners cw = cached.computeWinners(lotteryPick);

    EXPECT_EQ(cw.five, bw.five);
    EXPECT_EQ(cw.four, bw.four);
    EXPECT_EQ(cw.three, bw.three);
    EXPECT_EQ(cw.two, bw.two);
}

TEST(CachedLotteryProcessor, EmptyPlayersProducesZeroWinners) {
    CachedLotteryProcessor cached;
    cached.preComputeWinners({});
    auto w = cached.computeWinners(pick(1,2,3,4,5));

    EXPECT_EQ(w.five, 0);
    EXPECT_EQ(w.four, 0);
    EXPECT_EQ(w.three, 0);
    EXPECT_EQ(w.two, 0);
}

TEST(CachedLotteryProcessor, SinglePlayerExactMatch) {
    CachedLotteryProcessor cached;
    cached.preComputeWinners({ pick(1,2,3,4,5) });
    auto w = cached.computeWinners(pick(1,2,3,4,5));

    EXPECT_EQ(w.five, 1);
    EXPECT_EQ(w.four, 0);
    EXPECT_EQ(w.three, 0);
    EXPECT_EQ(w.two, 0);
}

TEST(CachedLotteryProcessor, DuplicatePlayersAccumulateCorrectly) {
    CachedLotteryProcessor cached;
    std::vector<std::array<uint8_t,5>> players;

    for (int i = 0; i < 10; ++i) {
        players.push_back(pick(5,4,3,2,1));
    }

    cached.preComputeWinners(players);
    auto w = cached.computeWinners(pick(1,2,3,4,5));

    EXPECT_EQ(w.five, 10);
    EXPECT_EQ(w.four, 0);
    EXPECT_EQ(w.three, 0);
    EXPECT_EQ(w.two, 0);
}

TEST(CachedLotteryProcessor, MixedMatchCategories) {
    CachedLotteryProcessor cached;

    std::vector<std::array<uint8_t,5>> players = {
        pick(1,2,3,4,5),  // five
        pick(1,2,3,4,6),  // four
        pick(1,2,3,4,6),  // four duplicate
        pick(1,2,3,7,8),  // three
        pick(1,2,9,10,11) // two
    };

    cached.preComputeWinners(players);
    auto w = cached.computeWinners(pick(1,2,3,4,5));

    EXPECT_EQ(w.five, 1);
    EXPECT_EQ(w.four, 2);
    EXPECT_EQ(w.three, 1);
    EXPECT_EQ(w.two, 1);
}

TEST(CachedLotteryProcessor, GenerateCombinationsValidSizes) {
    CachedLotteryProcessor cached;
    auto nums = pick(1,2,3,4,5);

    EXPECT_EQ(cached.test_generateCombinations(nums,2).size(), 10);
    EXPECT_EQ(cached.test_generateCombinations(nums,3).size(), 10);
    EXPECT_EQ(cached.test_generateCombinations(nums,4).size(), 5);
}

TEST(CachedLotteryProcessor, GenerateCombinationsInvalidSizesThrow) {
    CachedLotteryProcessor cached;
    auto nums = pick(1,2,3,4,5);

    EXPECT_THROW(cached.test_generateCombinations(nums,1), std::invalid_argument);
    EXPECT_THROW(cached.test_generateCombinations(nums,5), std::invalid_argument);
}

TEST(CachedLotteryProcessor, EncodeKeyNormalizesInputOrder) {
    CachedLotteryProcessor cached;

    auto sorted = pick(1,2,3,4,5);
    auto unsorted = pick(5,1,4,2,3);

    auto k1 = cached.test_encodeKey(sorted);
    auto k2 = CachedLotteryProcessor::test_getHashKeyFromNumbersCombination(unsorted);

    EXPECT_EQ(k1, k2);
}

TEST(CachedLotteryProcessor, PreComputeWinnersUpdatesCacheCorrectly) {
    CachedLotteryProcessor cached;
    cached.preComputeWinners({});

    auto playersNumbers = std::vector<std::array<uint8_t,5>>{pick(5,4,3,2,1), pick(7,6,8,9,10)};

    cached.preComputeWinners(playersNumbers);

    for (auto& playerNumbers : playersNumbers) {
        std::sort(playerNumbers.begin(), playerNumbers.end());
        EXPECT_EQ(cached.test_countMatches(playerNumbers), 1);
    }
}

TEST(CachedLotteryProcessor, ThreadCountZeroWhenNoWork) {
    CachedLotteryProcessor cached;
    EXPECT_EQ(cached.test_determineThreadCount(0), 0);
}

TEST(CachedLotteryProcessor, ChunkSizeZeroWhenThreadsZero) {
    CachedLotteryProcessor cached;
    EXPECT_EQ(cached.test_computeChunkSize(100, 0), 0);
}

TEST(CachedLotteryProcessor, ParallelProcessingPathExecuted) {
    CachedLotteryProcessor cached;
    std::vector<std::array<uint8_t,5>> players;

    for (int i = 0; i < 1000; ++i) {
        players.push_back(pick(1,2,3,4,5));
    }

    EXPECT_NO_THROW(cached.preComputeWinners(players));

    auto w = cached.computeWinners(pick(1,2,3,4,5));
    EXPECT_EQ(w.five, 1000);
    EXPECT_EQ(w.four, 0);
    EXPECT_EQ(w.three, 0);
    EXPECT_EQ(w.two, 0);
}
