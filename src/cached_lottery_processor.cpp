#include "cached_lottery_processor.h"

using namespace hungarian;

void CachedLotteryProcessor::preComputeWinners(const std::vector<std::array<uint8_t,5>>& players) {
    initializeCache();

    for (const auto& player : players) {
        preProcessPlayer(player);
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
}

void CachedLotteryProcessor::clearCache() {
    m_cache.clear();
}

void CachedLotteryProcessor::reserveCache() {
    // There are C(90,2) + C(90,3) + C(90,4) + C(90,5) combinations = 4005 + 117480 + 2555190 + 43949268 ~= 47M
    m_cache.reserve(47000000);
}

void CachedLotteryProcessor::preProcessPlayer(const std::array<uint8_t,5>& pick) {
    auto keys = generateEncodedCombinationKeys(pick);

    for (auto k : keys) {
        ++m_cache[k];
    }
}

std::vector<HashKey> CachedLotteryProcessor::generateEncodedCombinationKeys(const std::array<uint8_t,5>& pick) const {
    std::array<uint8_t,5> playerNumbers = pick;
    std::vector<HashKey> keys;

    std::sort(playerNumbers.begin(), playerNumbers.end());

    auto combinationsWith4Numbers = generateCombinations(playerNumbers, 4);
    auto combinationsWith3Numbers = generateCombinations(playerNumbers, 3);
    auto combinationsWith2Numbers = generateCombinations(playerNumbers, 2);

    keys.push_back(encodeKey(playerNumbers));

    for (const auto &c : combinationsWith4Numbers) keys.push_back(encodeKey(c));
    for (const auto &c : combinationsWith3Numbers) keys.push_back(encodeKey(c));
    for (const auto &c : combinationsWith2Numbers) keys.push_back(encodeKey(c));

    return keys;
}

uint32_t CachedLotteryProcessor::countMatches(const std::array<uint8_t,5>& key) const {
    auto encodedKey = encodeKey(key);
    auto it = m_cache.find(encodedKey);
    return it != m_cache.end() ? it->second : 0;
}

std::vector<std::array<uint8_t,5>> CachedLotteryProcessor::generateCombinations(const std::array<uint8_t,5>& numbers, int combinationSize) const {
    if (combinationSize < 2 || combinationSize > 4) {
        throw std::invalid_argument("combinationSize must be between 2 and 4");
    }

    switch (combinationSize) {
        case 2: 
            return generateCombinationsOf2(numbers);
        case 3:
            return generateCombinationsOf3(numbers);
        default:
            return generateCombinationsOf4(numbers);
    }
}

std::vector<std::array<uint8_t,5>> CachedLotteryProcessor::generateCombinationsOf2(const std::array<uint8_t,5>& numbers) const {
    std::vector<std::array<uint8_t,5>> result;
    std::array<uint8_t,5> combination{};
    const size_t offset = 3; // 5 - 2

    for (size_t i = 0; i < 5; ++i) {
        for (size_t j = i + 1; j < 5; ++j) {
            combination[offset] = numbers[i];
            combination[offset+1] = numbers[j];
            result.push_back(combination);
        }
    }

    return result;
}

std::vector<std::array<uint8_t,5>> CachedLotteryProcessor::generateCombinationsOf3(const std::array<uint8_t,5>& numbers) const {
    std::vector<std::array<uint8_t,5>> result;
    std::array<uint8_t,5> combination{};
    const size_t offset = 2; // 5 - 3

    for (size_t i = 0; i < 5; ++i) {
        for (size_t j = i + 1; j < 5; ++j) {
            for (size_t k = j + 1; k < 5; ++k) {
                combination[offset] = numbers[i];
                combination[offset+1] = numbers[j];
                combination[offset+2] = numbers[k];
                result.push_back(combination);
            }
        }
    }

    return result;
}

std::vector<std::array<uint8_t,5>> CachedLotteryProcessor::generateCombinationsOf4(const std::array<uint8_t,5>& numbers) const {
    std::vector<std::array<uint8_t,5>> result;
    std::array<uint8_t,5> combination{};
    const size_t offset = 1; // 5 - 4

    for (size_t i = 0; i < 5; ++i) {
        for (size_t j = i + 1; j < 5; ++j) {
            for (size_t k = j + 1; k < 5; ++k) {
                for (size_t l = k + 1; l < 5; ++l) {
                    combination[offset] = numbers[i];
                    combination[offset+1] = numbers[j];
                    combination[offset+2] = numbers[k];
                    combination[offset+3] = numbers[l];
                    result.push_back(combination);
                }
            }
        }
    }

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
