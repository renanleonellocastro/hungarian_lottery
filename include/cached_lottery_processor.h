/**
 * @file cached_lottery_processor.h
 * @brief Cached lottery processor that caches all possible lottery combinations.
 * and counts players having those combinations.
 */
#pragma once

#include "i_lottery_processor.h"

#include <array>
#include <vector>
#include <memory>
#include <cstdint>
#include <algorithm>
#include <stdexcept>
#include <unordered_map>

namespace hungarian {

using HashKey = uint64_t;

/**
 * @brief Cached processor that indexes all possible loterry combinations using a hash key.
 *
 * The cache maps encoded combination keys to counts of players having those combinations.
 * This allows for efficient computation of winners by looking up pre-computed counts.
 */
class CachedLotteryProcessor : public ILotteryProcessor {
public:

    /**
     * @brief Pre-compute winners by populating the cache with player combinations.
     * 
     * This method processes the list of players and their selected numbers,
     * generating all possible combinations for each player and updating the cache
     * with the counts of players having those combinations.
     */
    void preComputeWinners(const std::vector<std::array<uint8_t,5>>& players) override;

    /**
     * @brief Compute the winners for a given pick using the pre-computed cache.
     * 
     * This method calculates the number of winners for each prize category
     * (two, three, four, five matches) based on the provided pick and the
     * pre-computed cache of player combinations.
     * 
     * @param pick The selected numbers for which to compute winners.
     * @return A Winners struct containing the counts of winners for each category.
     */
    Winners computeWinners(const std::array<uint8_t, 5> &pick) const override;

#ifdef UNIT_TESTING
    // Test-only helpers
    std::vector<std::array<uint8_t,5>> test_generateCombinations(const std::array<uint8_t,5>& numbers, int size) const { return generateCombinations(numbers, size); }
    HashKey test_encodeKey(const std::array<uint8_t,5>& sortedNumbers) const { return encodeKey(sortedNumbers); }
    uint32_t test_countMatches(const std::array<uint8_t,5>& key) const { return countMatches(key); }
    void test_preProcessPlayer(const std::array<uint8_t,5>& pick) { preProcessPlayer(pick); }
    static HashKey test_getHashKeyFromNumbersCombination(const std::array<uint8_t,5>& combination) { return getHashKeyFromNumbersCombination(combination); }
#endif

private:
    /**
     * @brief Cache mapping encoded combination keys to counts.
     * 
     * The key is a uint64_t formed by encoding the sorted combination of numbers.
     * The value is the count of players having that combination.
     */
    std::unordered_map<HashKey,uint32_t> m_cache;

    /**
     * @internal
     * @brief Initializes the cache with all possible combinations available in the lottery.
     * 
     * This method is called during preComputeWinners to set up the cache structure.
     * It clears any existing data and reserves space for expected combinations.
     * 
     */
    void initializeCache();

    /**
     * @internal
     * @brief Clears the cache of all stored combinations and counts.
     * 
     * This method resets the cache to an empty state.
     */
    void clearCache();

    /**
     * @internal
     * @brief Reserves space in the cache for all possible combinations.
     * 
     * This method pre-allocates space in the cache to optimize performance
     * and minimize rehashing during population.
     */
    void reserveCache();

    /**
     * @internal
     * @brief Pre-process a player's numbers into the cache.
     * 
     * This method generates all combinations for the given player's numbers
     * and updates the cache counts accordingly.
     * 
     * @param pick The player's selected numbers as an array of 5 uint8_t values.
     */
    void preProcessPlayer(const std::array<uint8_t,5>& pick);
    /**
     * @brief Generate encoded keys for all combinations of a player's numbers.
     *
     * Helper that builds all combinations (5,4,3,2) for a sorted player pick and
     * returns their encoded keys so the cache can be updated. Keeps
     * `preProcessPlayer` short and focused.
     *
     * @param pick Player's numbers (must be 5 elements).
     * @return std::vector<HashKey> Encoded keys for combinations.
     */
    std::vector<HashKey> generateEncodedCombinationKeys(const std::array<uint8_t,5>& pick) const;

    /**
     * @internal
     * @brief Counts how many players have the exact matching combination represented by the key.
     * 
     * This method looks up the cache for the given encoded key and returns the count of players
     * who have that exact combination.
     * 
     * @param key The encoded combination key as an array of 5 uint8_t values.
     * @return The count of players with the matching combination.
     */
    uint32_t countMatches(const std::array<uint8_t,5>& key) const;

    /**
     * @internal
     * @brief Generate all combinations, when the order does not matter, of a given size from the provided numbers.
     *
     * This helper generates all combinations of `combinationSize`, when the order does not matter, from
     * the input `numbers` array.
     *
     * @param numbers The input array of numbers to generate combinations from.
     * @param combinationSize The size of each combination to generate.
     * @return A vector of combinations, each represented as an array of uint8_t with 5 positions.
     * It uses "0" for unused positions.
     */
    std::vector<std::array<uint8_t,5>> generateCombinations(const std::array<uint8_t,5>& numbers, int combinationSize) const;

    /**
     * @brief Internal helpers for specific combination sizes.
     */
    std::vector<std::array<uint8_t,5>> generateCombinationsOf2(const std::array<uint8_t,5>& numbers) const;
    std::vector<std::array<uint8_t,5>> generateCombinationsOf3(const std::array<uint8_t,5>& numbers) const;
    std::vector<std::array<uint8_t,5>> generateCombinationsOf4(const std::array<uint8_t,5>& numbers) const;

    /**
     * @internal
     * @brief Compute winners with two matching numbers.
     * 
     * This method updates the winners struct by counting players with exactly two matching numbers.
     * @param lotteryNumbers The lottery numbers to compare against.
     * @param winners The Winners struct to update.
     */
    void computeWinnersWithTwoMatchingNumbers(std::array<uint8_t,5>& lotteryNumbers, Winners &winners) const;

    /**
     * @internal
     * @brief Compute winners with three matching numbers.
     * 
     * This method updates the winners struct by counting players with exactly three matching numbers.
     * @param lotteryNumbers The lottery numbers to compare against.
     * @param winners The Winners struct to update.
     */
    void computeWinnersWithThreeMatchingNumbers(std::array<uint8_t,5>& lotteryNumbers, Winners &winners) const;

    /**
     * @internal
     * @brief Compute winners with four matching numbers.
     * 
     * This method updates the winners struct by counting players with exactly four matching numbers.
     * @param lotteryNumbers The lottery numbers to compare against.
     * @param winners The Winners struct to update.
     */
    void computeWinnersWithFourMatchingNumbers(std::array<uint8_t,5>& lotteryNumbers, Winners &winners) const;

    /**
     * @internal
     * @brief Compute winners with five matching numbers.
     * 
     * This method updates the winners struct by counting players with exactly five matching numbers.
     * @param lotteryNumbers The lottery numbers to compare against.
     * @param winners The Winners struct to update.
     */
    void computeWinnersWithFiveMatchingNumbers(std::array<uint8_t,5>& lotteryNumbers, Winners &winners) const;

    /**
     * @internal
     * @brief Sort and Encode a combination of numbers into a HashKey.
     * 
     * This method takes an array of numbers, sorts and encodes it into a single uint64_t key.
     * The encoding ensures that each sorted unique combination maps to a unique key.
     * 
     * @param combination An array of numbers representing the combination.
     * @return The encoded hash key as a uint64_t.
     */
    static HashKey getHashKeyFromNumbersCombination(const std::array<uint8_t,5>& combinationOfNumbers);

    /**
     * @internal
     * @brief Encode a sorted array of numbers into a HashKey.
     * 
     * This method encodes a sorted array of numbers into a single uint64_t key.
     * The encoding ensures that each sorted unique combination maps to a unique key.
     * 
     * @param sortedNumbers An array of sorted numbers to encode.
     * @return The encoded hash key as a uint64_t.
     */
    static HashKey encodeKey(const std::array<uint8_t,5>& sortedNumbers);
};

} // namespace hungarian
