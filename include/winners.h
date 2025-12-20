/**
 * @file winners.h
 * @brief Winners struct for lottery results.
 */
#pragma once

#include <cstdint>

namespace hungarian {

/**
 * @brief Result counts for match groups.
 */
struct Winners {
    uint32_t five{};
    uint32_t four{};
    uint32_t three{};
    uint32_t two{};
};

} // namespace hungarian
