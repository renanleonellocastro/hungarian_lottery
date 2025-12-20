#include "console_reader.h"
#include <iostream>
#include <stdexcept>
#include "logging_shim.h"

using namespace hungarian;

/**
 * @internal
 * @brief Validate a single lottery pick value read from stdin.
 *
 * Logs and throws if the value is out of allowed bounds.
 *
 * @param value The integer value read from stdin.
 */
static void validate_pick_value(int value) {
    if (value < 1 || value > 90) {
        LOG(FATAL) << "Lottery pick number out of range: " << value;
        throw std::runtime_error("lottery pick out of range");
    }
}

void ConsoleReader::readLotteryPick() {
    std::array<int,5> temp{};

    if (!(std::cin >> temp[0] >> temp[1] >> temp[2] >> temp[3] >> temp[4])) {
        LOG(FATAL) << "Failed to read 5 numbers from stdin";
        throw std::runtime_error("failed to read lottery pick");
    }

    std::array<uint8_t,5> lotteryPick{};
    for (int i = 0; i < 5; ++i) {
        validate_pick_value(temp[i]);
        lotteryPick[i] = static_cast<uint8_t>(temp[i]);
    }

    setMembers(lotteryPick);
    LOG(DEBUG) << "Read lottery pick: " << lotteryPick[0] << ' ' << lotteryPick[1] << ' ' << lotteryPick[2] << ' ' << lotteryPick[3] << ' ' << lotteryPick[4];
}

void ConsoleReader::setMembers(const std::array<uint8_t,5>& lotteryPick) {
    m_lotteryPick = lotteryPick;
}
