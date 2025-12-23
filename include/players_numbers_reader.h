/**
 * @file players_numbers_reader.h
 * @brief Reader for players' numbers files.
 */
#pragma once

#include <string>
#include <vector>
#include <array>
#include <sstream>
#include <cstdint>

namespace hungarian {

/**
 * @brief Read player's numbers from a file.
 *
 * Each line must contain exactly 5 integers between 1 and 90 separated by spaces.
 */
class PlayersNumbersReader {
public:

    /**
     * @brief Read file and populate internal structure.
     * @param filepath Path to input file.
     * @throws std::runtime_error on parse/validation errors.
     */
    void readFile(const std::string& filepath);

    /**
     * @brief Access the parsed players numbers as vector of 5-length vectors.
     */
    const std::vector<std::array<uint8_t,5>>& players() const noexcept { return m_players; }

private:
    /**
     * @brief Hold parsed player's numbers.
     * 
     * Each inner vector has size 5.
     */
    std::vector<std::array<uint8_t,5>> m_players;

    /**
     * @brief Parse a single line from the input file into 5 numbers.
     *
     * This helper parses a textual line, validates each parsed integer and
     * returns a vector of 5 numbers. It throws on parse or validation errors.
     *
     * @param line Raw text line from input file.
     * @param lineNumber Line number (for error reporting).
     * @return An std::array<uint8_t,5> containing exactly five parsed values.
     * @throws std::runtime_error on parse/validation errors.
     */
    std::array<uint8_t,5> parseLine(const std::string& line, size_t lineNumber) const;

    /**
     * @brief Parse numbers from a line stream into an array of five numbers.
     *
     * Helper used by `parseLine` to keep the public-facing method short and
     * focused. Validates each parsed integer and enforces exactly five values.
     *
     * @param lineStream Input line stream positioned at the start of data.
     * @param lineNumber Line number for error reporting.
     * @return std::array<uint8_t,5> Parsed five numbers.
     */
    std::array<uint8_t,5> parseNumbersFromStream(std::istringstream& lineStream, size_t lineNumber) const;

    /**
     * @brief Fill an array of five numbers from a line stream.
     *
     * Low-level helper used by `parseNumbersFromStream` to perform the
     * extraction loop. Keeps higher-level methods short and focused.
     */
    std::array<uint8_t,5> fillNumbersFromStream(std::istringstream& lineStream, size_t lineNumber) const;

    /**
     * @brief Validate a single integer value is within allowed range.
     *
     * Ensures the provided value is within [1,90].
     * Logs and throws on failure.
     *
     * @param value Integer to validate.
     * @param lineNumber Line number for error messages.
     * @throws std::runtime_error on validation failure.
     */
    void validateValue(int value, size_t lineNumber) const;

    /**
     * @brief Validate that the given 5 numbers are unique.
     *
     * Sorts a local copy and ensures there are no duplicate numbers in the
     * provided pick.
     *
     * @param numbers The five numbers to validate for uniqueness.
     * @param lineNumber Line number for error messages.
     */
    void validateUnique(const std::array<uint8_t,5>& numbers, size_t lineNumber) const;
};

} // namespace hungarian
