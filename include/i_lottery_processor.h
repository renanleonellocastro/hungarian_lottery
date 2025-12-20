/**
 * @file lottery_processor.h
 * @brief Abstract lottery processor template and Winners struct.
 */
#pragma once

#include "winners.h"

#include <array>
#include <vector>

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
