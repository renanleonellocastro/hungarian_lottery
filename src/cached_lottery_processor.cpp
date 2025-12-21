#include "cached_lottery_processor.h"

using namespace hungarian;

void CachedLotteryProcessor::preComputeWinners(const std::vector<std::vector<uint8_t>>& players) {
    initializeCache();

    for (const auto& player : players) {
        std::array<uint8_t,5> playerNumbersArray{};
        std::copy_n(player.begin(), 5, playerNumbersArray.begin());
        preProcessPlayer(playerNumbersArray);
    }
}

Winners CachedLotteryProcessor::computeWinners(const std::array<uint8_t,5>& pick) const {
    Winners winners{};
    std::array<uint8_t,5> lotteryNumbers = pick;

    std::sort(lotteryNumbers.begin(), lotteryNumbers.end());

    computeWinnersWithFiveMatchingNumbers(lotteryNumbers, winners);
    computeWinnersWithFourMatchingNumbers(lotteryNumbers, winners);
    computeWinnersWithThreeMatchingNumbers(lotteryNumbers, winners);
    computeWinnersWithTwoMatchingNumbers(lotteryNumbers, winners);

    return winners;
}

void CachedLotteryProcessor::computeWinnersWithTwoMatchingNumbers(std::array<uint8_t,5>& lotteryNumbers, Winners &winners) const
{
    auto combinationsOf2Numbers = generateCombinations(lotteryNumbers, 2);

    for (const auto &combination : combinationsOf2Numbers) {
        winners.two += countMatches(combination);
    }

    winners.two -= 10 * winners.five + 6 * winners.four + 3 * winners.three;
}

void CachedLotteryProcessor::computeWinnersWithThreeMatchingNumbers(std::array<uint8_t,5>& lotteryNumbers, Winners &winners) const
{
    auto combinationsOf3Numbers = generateCombinations(lotteryNumbers, 3);

    for (const auto& combination : combinationsOf3Numbers) {
        winners.three += countMatches(combination);
    }

    winners.three -= 10 * winners.five + 4 * winners.four;
}

void CachedLotteryProcessor::computeWinnersWithFourMatchingNumbers(std::array<uint8_t,5>& lotteryNumbers, Winners &winners) const
{
    auto combinationsOf4Numbers = generateCombinations(lotteryNumbers, 4);

    for (const auto &combination : combinationsOf4Numbers) {
        winners.four += countMatches(combination);
    }

    winners.four -= 5 * winners.five;
}

void CachedLotteryProcessor::computeWinnersWithFiveMatchingNumbers(std::array<uint8_t,5>& lotteryNumbers, Winners &winners) const
{
    winners.five = countMatches(lotteryNumbers);
}

void CachedLotteryProcessor::initializeCache() {
    clearCache();
    reserveCache();

    auto insertZero = [this](const std::array<uint8_t,5>& combination) {
        m_cache.emplace(encodeKey(combination), 0u);
    };

    CombinationGenerator<2>::run(insertZero);
    CombinationGenerator<3>::run(insertZero);
    CombinationGenerator<4>::run(insertZero);
    CombinationGenerator<5>::run(insertZero);
}

void CachedLotteryProcessor::clearCache() {
    m_cache.clear();
}

void CachedLotteryProcessor::reserveCache() {
    // There are C(90,2) + C(90,3) + C(90,4) + C(90,5) combinations = 4005 + 117480 + 2555190 + 43949268 ~= 47M
    m_cache.reserve(47000000);
}

void CachedLotteryProcessor::preProcessPlayer(const std::array<uint8_t,5>& pick) {
    std::array<uint8_t,5> playerNumbers = pick;
    std::vector<std::array<uint8_t,5>> combinations;

    std::sort(playerNumbers.begin(), playerNumbers.end());

    auto combinationsOf4Numbers = generateCombinations(playerNumbers, 4);
    auto combinationsOf3Numbers = generateCombinations(playerNumbers, 3);
    auto combinationsOf2Numbers = generateCombinations(playerNumbers, 2);
    
    combinations.push_back(playerNumbers);
    combinations.insert(combinations.end(), combinationsOf4Numbers.begin(), combinationsOf4Numbers.end());
    combinations.insert(combinations.end(), combinationsOf3Numbers.begin(), combinationsOf3Numbers.end());
    combinations.insert(combinations.end(), combinationsOf2Numbers.begin(), combinationsOf2Numbers.end());

    for (const auto& combination : combinations) {
        ++m_cache[encodeKey(combination)];
    }
}

uint32_t CachedLotteryProcessor::countMatches(const std::array<uint8_t,5>& key) const {
    return m_cache.at(encodeKey(key));
}

std::vector<std::array<uint8_t, 5>> CachedLotteryProcessor::generateCombinations(const std::array<uint8_t, 5>& numbers, int combinationSize) const {
    if (combinationSize < 2 || combinationSize > 4) {
        throw std::invalid_argument("combinationSize must be between 2 and 4");
    }

    std::vector<std::array<uint8_t, 5>> result;
    std::array<uint8_t, 5> current{}; // all zeros
    std::size_t depth = 0;

    const std::size_t offset = 5 - static_cast<std::size_t>(combinationSize);

    const auto backtrack = [&](auto&& self, std::size_t start) -> void {
        if (depth == static_cast<std::size_t>(combinationSize)) {
            result.push_back(current);
            return;
        }

        for (std::size_t i = start; i < numbers.size(); ++i) {
            current[offset + depth++] = numbers[i];
            self(self, i + 1);
            current[offset + --depth] = 0; // restore
        }
    };

    backtrack(backtrack, 0);
    return result;
}

HashKey CachedLotteryProcessor::getHashKeyFromNumbersCombination(const std::array<uint8_t,5>& combinationOfNumbers) {
    std::array<uint8_t,5> sortedCombination = combinationOfNumbers;
    std::sort(sortedCombination.begin(), sortedCombination.end());
    return encodeKey(sortedCombination);
}

HashKey CachedLotteryProcessor::encodeKey(const std::array<uint8_t,5>& sortedNumbers) {
    return (HashKey(sortedNumbers[0]) << 32) |
           (HashKey(sortedNumbers[1]) << 24) |
           (HashKey(sortedNumbers[2]) << 16) |
           (HashKey(sortedNumbers[3]) <<  8) |
            HashKey(sortedNumbers[4]);
}
