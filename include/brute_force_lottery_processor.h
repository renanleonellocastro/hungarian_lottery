/**
 * @file brute_force_lottery_processor.h
 * @brief Brute-force lottery processor calculate winners by direct comparison.
 */
#pragma once

#include "i_lottery_processor.h"

#include <vector>
#include <memory>
#include <array>
#include <algorithm>

namespace hungarian {

/**
 * @brief Brute-force lottery processor implementation.
 * This processor directly compares each player's numbers against the
 * lottery pick to count matches.
 */
class BruteForceLotteryProcessor : public ILotteryProcessor {
public:
    /**
     * @brief Preprocess players by storing them directly.
     * @param players Vector of players, each with their selected numbers.
     */
    void preComputeWinners(const std::vector<std::vector<uint8_t>>& players) override;

    /**
     * @brief Compute winners by direct comparison of each player's numbers
     * against the lottery pick.
     * @param pick The lottery pick (5 numbers).
     * @return Winners struct with counts of matches.
     */
    Winners computeWinners(const std::array<uint8_t, 5> &pick) const override;

private:
    /**
     * @brief Processed container produced by `preComputeWinners`.
     *
     * This will be populated inside `preComputeWinners` so that `computeWinners()`
     * can read from it. For the brute-force processor, this is simply
     * a vector of players; for other processors it may be an index or cache structure.
     */
    std::vector<std::vector<uint8_t>> m_container{};

    /**
     * @internal
     * @brief Count how many numbers from `player` exist in `pick`.
     *
     * Simple helper used by the brute-force implementation. This performs a
     * linear search for each player's value in the pick (5 items) and returns
     * the match count.
     *
     * @param pick The lottery pick (5 numbers).
     * @param player Player's selected numbers (5 numbers).
     * @return Number of matching values (0..5).
     */
    static int countMatches(const std::array<uint8_t,5>& pick, const std::vector<uint8_t>& player);

    /**
     * @internal
     * @brief Update Winners struct based on number of matches.
     *
     * Increments the appropriate field in the Winners struct based on the
     * number of matches (2, 3, 4, or 5).
     *
     * @param matches Number of matches (0..5).
     * @param w Reference to Winners struct to update.
     */
    void computeWinner(int matches, Winners &w) const;
};

} // namespace hungarian
