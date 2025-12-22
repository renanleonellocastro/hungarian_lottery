/**
 * @file console_reader.h
 * @brief Read lottery pick from stdin.
 */
#pragma once

#include <memory>
#include <cstdint>

namespace hungarian {

class ConsoleReader {
public:
    /**
     * @brief Reads exactly 5 numbers from stdin into internal fields.
     * @throws std::runtime_error on parse/validation errors.
     */
    void readLotteryPick();

    /**
     * @brief Lottery pick numbers.
     * 
     * Holds the lottery pick numbers read from stdin after `readLotteryPick()`.
     * @return std::array<uint8_t,5> Lottery pick numbers.
     */
    const std::array<uint8_t,5>& getLotteryPick() const noexcept { return m_lotteryPick; }

private:
    std::array<uint8_t,5> m_lotteryPick;

    /**
     * @internal
     * @brief Assign lottery pick values to member variable.
     *
     * Small helper used to assign values to member variable after validation.
     *
     * @param lotteryPick The 5 lottery pick numbers.
     */
    void setMembers(const std::array<uint8_t,5>& lotteryPick);
};

} // namespace hungarian
