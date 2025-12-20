/**
 * @file players_numbers_reader.h
 * @brief Reader for player's numbers files.
 */
#pragma once

#include <string>
#include <vector>
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
    const std::vector<std::vector<uint8_t>>& players() const noexcept { return m_players; }

private:
    /**
     * @brief Hold parsed player's numbers.
     * 
     * Each inner vector has size 5.
     */
    std::vector<std::vector<uint8_t>> m_players;

    /**
     * @brief Parse a single line from the input file into 5 numbers.
     *
     * This helper parses a textual line, validates each parsed integer and
     * returns a vector of 5 numbers. It throws on parse or validation errors.
     *
     * @param line Raw text line from input file.
     * @param lineno Line number (for error reporting).
     * @return A vector<uint8_t> containing exactly five parsed values.
     * @throws std::runtime_error on parse/validation errors.
     */
    std::vector<uint8_t> parseLine(const std::string& line, size_t lineno) const;

    /**
     * @brief Validate a single integer value is within allowed range.
     *
     * Ensures the provided value is within [1,90].
     * Logs and throws on failure.
     *
     * @param value Integer to validate.
     * @param lineno Line number for error messages.
     * @throws std::runtime_error on validation failure.
     */
    void validateValue(int value, size_t lineno) const;

    /**
     * @brief Validate that the given 5 numbers are unique.
     *
     * Sorts a local copy and ensures there are no duplicate numbers in the
     * provided pick.
     *
     * @param numbers The five numbers to validate for uniqueness.
     * @param lineno Line number for error messages.
     */
    void validateUnique(const std::vector<uint8_t>& numbers, size_t lineno) const;
};

} // namespace hungarian
