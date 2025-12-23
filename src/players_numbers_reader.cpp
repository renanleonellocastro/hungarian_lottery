#include "players_numbers_reader.h"

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include <glog/logging.h>

using namespace hungarian;

std::array<uint8_t,5> PlayersNumbersReader::parseLine(const std::string& line, size_t lineNumber) const {
    std::istringstream lineStream(line);
    return parseNumbersFromStream(lineStream, lineNumber);
}

std::array<uint8_t,5> PlayersNumbersReader::parseNumbersFromStream(std::istringstream &lineStream, size_t lineNumber) const {
    auto numbers = fillNumbersFromStream(lineStream, lineNumber);
    validateUnique(numbers, lineNumber);
    return numbers;
}

std::array<uint8_t,5> PlayersNumbersReader::fillNumbersFromStream(std::istringstream &lineStream, size_t lineNumber) const {
    std::array<uint8_t,5> numbers{};
    int parsedValue;
    size_t index = 0;

    while (lineStream >> parsedValue) {
        validateValue(parsedValue, lineNumber);

        if (index >= numbers.size()) {
            LOG(ERROR) << "Invalid numbers count on line " << lineNumber << " (expected " << numbers.size() << ").";
            throw std::runtime_error("invalid numbers count");
        }

        numbers[index++] = static_cast<uint8_t>(parsedValue);
    }

    if (index != numbers.size()) {
        LOG(ERROR) << "Invalid numbers count on line " << lineNumber << " (expected " << numbers.size() << ").";
        throw std::runtime_error("invalid numbers count");
    }

    return numbers;
}

void PlayersNumbersReader::validateValue(int value, size_t lineNumber) const {
    if (value < 1 || value > 90) {
        LOG(ERROR) << "Value out of range on line " << lineNumber << ": " << value;
        throw std::runtime_error("value out of range");
    }
}

void PlayersNumbersReader::validateUnique(const std::array<uint8_t,5>& numbers, size_t lineNumber) const {
    auto sortedNumbers = numbers;
    std::sort(sortedNumbers.begin(), sortedNumbers.end());

    if (std::unique(sortedNumbers.begin(), sortedNumbers.end()) != sortedNumbers.end()) {
        LOG(ERROR) << "Duplicate numbers found on line " << lineNumber;
        throw std::runtime_error("duplicate numbers in pick");
    }
}

void PlayersNumbersReader::readFile(const std::string& filepath) {
    std::string line;
    size_t lineNumber = 0;
    std::ifstream ifs(filepath);

    m_players.clear();

    if (!ifs) {
        LOG(ERROR) << "Failed to open input file: " << filepath;
        throw std::runtime_error("failed to open input file");
    }

    while (std::getline(ifs, line)) {
        ++lineNumber;
    
        if (line.empty()) {
            continue;
        }

        m_players.push_back(parseLine(line, lineNumber));
    }

    LOG(INFO) << "Read " << m_players.size() << " players from " << filepath;
}
