#include "console_reader.h"

#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <glog/logging.h>

using namespace hungarian;

/**
 * @brief Validator for individual pick number.
 * 
 * Validates that a given integer is within the allowed range [1,90].
 * Logs and throws on failure.
 * 
 * @param value Integer to validate.
 * @throws std::runtime_error on validation failure.
 */
static void validatePickNumberBetween1And90(int value) {
    if (value < 1 || value > 90) {
        LOG(ERROR) << "Lottery pick number out of range: " << value;
        throw std::runtime_error("lottery pick out of range");
    }
}

/**
 * @brief Validator for too many numbers in pick.
 * 
 * Validates if there were more than 5 numbers provided for a lottery pick.
 * Logs and throws on failure.
 * 
 * @param count Number of provided numbers.
 * @throws std::runtime_error on validation failure.
 */
static void validateTooManyNumbersProvidedForLotteryPick(int count) {
    if (count > 5) {
        LOG(ERROR) << "Too many numbers provided for lottery pick";
        throw std::runtime_error("too many numbers for lottery pick");
    }
}

/**
 * @brief Validator for too few numbers in pick.
 * 
 * Validates if there were less than 5 numbers provided for a lottery pick.
 * 
 * @param count Number of provided numbers.
 * @throws std::runtime_error on validation failure.
 */
static void validateTooFewNumbersProvidedForLotteryPick(int count) {
    if (count < 5) {
        LOG(ERROR) << "Too few numbers provided for lottery pick";
        throw std::runtime_error("too few numbers for lottery pick");
    }
}

/**
 * @brief Global validator for unique numbers in pick.
 * 
 * Validates that all numbers in the provided lottery pick are unique.
 * Logs and throws on failure.
 * 
 * @param pick The lottery pick numbers to validate.
 * @throws std::runtime_error on validation failure.
 */
static void validateUnique(const std::array<uint8_t,5>& pick) {
    auto sortedNumbers = pick;
    std::sort(sortedNumbers.begin(), sortedNumbers.end());

    if (std::unique(sortedNumbers.begin(), sortedNumbers.end()) != sortedNumbers.end()) {
        LOG(ERROR) << "Duplicate numbers found on pick";
        throw std::runtime_error("duplicate numbers in pick");
    }
}

/**
 * @brief Read a line from stdin.
 * 
 * This helper reads a string line from stdin.
 * 
 * @return std::string Temporary string with read numbers.
 * @throws std::runtime_error on read failure.
 */
static std::string readLineFromStdin() {
    std::string temp;

    if (!(std::getline(std::cin, temp))) {
        LOG(ERROR) << "Failed to read line from stdin";
        throw std::runtime_error("failed to read lottery pick");
    }

    return temp;
}

/**
 * @brief Converts and validates temporary read line into uint8_t array.
 * 
 * This helper takes a temporary string, validates if it contains valid lottery pick numbers
 * separated by spaces and converts them into a uint8_t array suitable for lottery pick storage.
 * 
 * @param temp Temporary string line read from stdin.
 * @return std::array<uint8_t,5> Validated and converted lottery pick numbers.
 * @throws std::runtime_error on validation failure.
 */
static std::array<uint8_t,5> convertAndValidateReadLine(const std::string& temp) {
    int value;
    int count = 0;
    std::istringstream iss(temp);
    std::array<uint8_t,5> lotteryPick{};

    while (iss >> value) {
        validatePickNumberBetween1And90(value);
        lotteryPick[count++%5] = static_cast<uint8_t>(value);
    }

    validateTooManyNumbersProvidedForLotteryPick(count);
    validateTooFewNumbersProvidedForLotteryPick(count);
    validateUnique(lotteryPick);

    return lotteryPick;
}

void ConsoleReader::readLotteryPick() {
    auto temp = readLineFromStdin();
    auto lotteryPick = convertAndValidateReadLine(temp);
    setMembers(lotteryPick);
    LOG(INFO) << "Read lottery pick: " << lotteryPick[0] << ' ' << lotteryPick[1] << ' ' << lotteryPick[2] << ' ' << lotteryPick[3] << ' ' << lotteryPick[4];
}

void ConsoleReader::setMembers(const std::array<uint8_t,5>& lotteryPick) {
    m_lotteryPick = lotteryPick;
}
