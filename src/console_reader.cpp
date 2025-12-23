#include "console_reader.h"

#include <iostream>
#include <stdexcept>
#include <glog/logging.h>

using namespace hungarian;

/**
 * @brief Global validator for pick values.
 * 
 * Validates that a given integer is within the allowed range [1,90].
 * Logs and throws on failure.
 * 
 * @param value Integer to validate.
 * @throws std::runtime_error on validation failure.
 */
static void validatePickValue(int value) {
    if (value < 1 || value > 90) {
        LOG(ERROR) << "Lottery pick number out of range: " << value;
        throw std::runtime_error("lottery pick out of range");
    }
}

/**
 * @brief Reads exactly 5 numbers from stdin into a temporary array.
 * 
 * This helper reads 5 integers from standard input and returns them
 * in an array. It throws on read failure.
 * 
 * @return std::array<int,5> Temporary array with read numbers.
 * @throws std::runtime_error on read failure.
 */
static std::array<int,5> readTempNumbersFromStdin() {
    std::array<int,5> temp{};

    if (!(std::cin >> temp[0] >> temp[1] >> temp[2] >> temp[3] >> temp[4])) {
        LOG(ERROR) << "Failed to read 5 numbers from stdin";
        throw std::runtime_error("failed to read lottery pick");
    }

    return temp;
}

/**
 * @brief Converts and validates temporary read numbers into uint8_t array.
 * 
 * This helper takes a temporary array of integers, validates each value,
 * and converts them into a uint8_t array suitable for lottery pick storage.
 * 
 * @param temp Temporary array of integers read from stdin.
 * @return std::array<uint8_t,5> Validated and converted lottery pick numbers.
 * @throws std::runtime_error on validation failure.
 */
static std::array<uint8_t,5> convertAndValidateTemp(const std::array<int,5>& temp) {
    std::array<uint8_t,5> lotteryPick{};

    for (int i = 0; i < 5; ++i) {
        validatePickValue(temp[i]);
        lotteryPick[i] = static_cast<uint8_t>(temp[i]);
    }

    return lotteryPick;
}

void ConsoleReader::readLotteryPick() {
    auto temp = readTempNumbersFromStdin();
    auto lotteryPick = convertAndValidateTemp(temp);
    setMembers(lotteryPick);
    LOG(INFO) << "Read lottery pick: " << lotteryPick[0] << ' ' << lotteryPick[1] << ' ' << lotteryPick[2] << ' ' << lotteryPick[3] << ' ' << lotteryPick[4];
}

void ConsoleReader::setMembers(const std::array<uint8_t,5>& lotteryPick) {
    m_lotteryPick = lotteryPick;
}
