/**
 * @file i_lottery_processor.h
 * @brief Interface for lottery processors.
 */
#pragma once

#include <array>
#include <vector>

#include "winners.h"

namespace hungarian {

/**
 * @brief Interface for lottery runtime polymorphism.
 *
 * This interface provides the virtual API that callers need without
 * exposing the concrete container type. Template-based processors
 * can inherit from this via `LotteryProcessor<...>`.
 */
class ILotteryProcessor {
public:
    virtual ~ILotteryProcessor() = default;

    /**
     * @brief Pre-compute winners for a set of players.
     * 
     * This virtual method allows derived classes to implement
     * pre-computation logic specific to their internal data structures.
     * @param players A vector of players' numbers, each represented as an array of 5 uint8_t values.
     */
    virtual void preComputeWinners(const std::vector<std::array<uint8_t,5>>& players) = 0;

    /**
     * @brief Compute winners for a given pick.
     * 
     * This virtual method allows derived classes to implement
     * winner computation logic specific to their internal data structures.
     * @param pick An array of 5 uint8_t values representing the lottery pick.
     * @return A Winners struct containing the count of winners by category.
     */
    virtual Winners computeWinners(const std::array<uint8_t,5>& pick) const = 0;
};

} // namespace hungarian
