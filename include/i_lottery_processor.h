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

    virtual void preComputeWinners(const std::vector<std::vector<uint8_t>>& players) = 0;
    virtual Winners computeWinners(const std::array<uint8_t,5>& pick) const = 0;
};

} // namespace hungarian
