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
using Cache = std::unordered_map<HashKey,uint32_t>;

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
    size_t test_determineThreadCount(size_t workSize) const { return determineThreadCount(workSize); }
    size_t test_computeChunkSize(size_t total, size_t threads) const { return computeChunkSize(total, threads); }
    static HashKey test_getHashKeyFromNumbersCombination(const std::array<uint8_t,5>& combination) { return getHashKeyFromNumbersCombination(combination); }
#endif

private:
    /**
     * @brief Cache mapping encoded combination keys to counts.
     * 
     * The key is a uint64_t formed by encoding the sorted combination of numbers.
     * The value is the count of players having that combination.
     */
    Cache m_cache;

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
     * @brief Determine Thread Count for Pre-computation.
     * 
     * This method calculates the optimal number of threads to use for pre-computation
     * based on the hardware concurrency available.
     * 
     * @param workSize The size of the work to be done.
     * @return The number of threads to use.
     */
    size_t determineThreadCount(size_t workSize) const;

    /**
     * @internal
     * @brief Compute Chunk Size for Threaded Work.
     * 
     * This method calculates the size of each chunk of work to be assigned to threads
     * during pre-computation.
     * 
     * @param total The total size of the work.
     * @param threads The number of threads to divide the work among.
     * @return The size of each chunk of work.
     */
    size_t computeChunkSize(size_t total, size_t threads) const;

    /**
     * @internal
     * @brief Pre-compute combinations in parallel using multiple threads.
     * 
     * This method divides the work of pre-computing combinations among multiple threads
     * to improve performance on large datasets.
     * 
     * @param players A vector of players' numbers, each represented as an array of 5 uint8_t values.
     * @param threadCount The number of threads to use for parallel processing.
     * @return A vector of Cache objects, each representing the cache computed by a thread.
     */
    std::vector<Cache> preComputeInParallel(const std::vector<std::array<uint8_t,5>>& players, size_t threadCount);

    /**
     * @internal
     * @brief Process a chunk of players to update the cache.
     * 
     * This method processes a subset of players' numbers and updates a local cache
     * with the counts of combinations for that chunk.
     * 
     * @param players A vector of players' numbers, each represented as an array of 5 uint8_t values.
     * @param begin The starting index of the chunk to process.
     * @param end The ending index of the chunk to process.
     * @return A Cache object representing the cache computed for the chunk.
     */
    Cache processPlayerChunk(const std::vector<std::array<uint8_t,5>>& players, size_t begin, size_t end) const;

    /**
     * @internal
     * @brief Merge local caches from multiple threads into the main cache.
     * 
     * This method combines the results from multiple local caches computed by different threads
     * into the main cache of the CachedLotteryProcessor.
     * 
     * @param locals A vector of Cache objects, each representing a local cache from a thread.
     */
    void mergeLocalCaches(std::vector<Cache>& locals);

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
