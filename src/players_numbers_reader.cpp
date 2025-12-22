#include "players_numbers_reader.h"

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include <glog/logging.h>

using namespace hungarian;

std::vector<uint8_t> PlayersNumbersReader::parseLine(const std::string& line, size_t lineno) const {
    std::istringstream line_stream(line);
    std::vector<uint8_t> numbers;
    int parsed_value;

    while (line_stream >> parsed_value) {
        validateValue(parsed_value, lineno);
        numbers.push_back(static_cast<uint8_t>(parsed_value));
    }

    if (numbers.size() != 5) {
        LOG(ERROR) << "Invalid numbers count on line " << lineno << " (expected 5).";
        throw std::runtime_error("invalid numbers count");
    }

    validateUnique(numbers, lineno);
    return numbers; // The compiler handles this optimally (NRVO or implicit move)
}

void PlayersNumbersReader::validateValue(int value, size_t lineno) const {
    if (value < 1 || value > 90) {
        LOG(ERROR) << "Value out of range on line " << lineno << ": " << value;
        throw std::runtime_error("value out of range");
    }
}

void PlayersNumbersReader::validateUnique(const std::vector<uint8_t>& numbers, size_t lineno) const {
    auto sorted_numbers = numbers;
    std::sort(sorted_numbers.begin(), sorted_numbers.end());

    if (std::unique(sorted_numbers.begin(), sorted_numbers.end()) != sorted_numbers.end()) {
        LOG(ERROR) << "Duplicate numbers found on line " << lineno;
        throw std::runtime_error("duplicate numbers in pick");
    }
}

void PlayersNumbersReader::readFile(const std::string& filepath) {
    std::string line;
    size_t lineno = 0;
    m_players.clear();
    std::ifstream ifs(filepath);

    if (!ifs) {
        LOG(ERROR) << "Failed to open input file: " << filepath;
        throw std::runtime_error("failed to open input file");
    }

    while (std::getline(ifs, line)) {
        ++lineno;
        if (line.empty()) continue;
        m_players.push_back(parseLine(line, lineno));
    }

    LOG(INFO) << "Read " << m_players.size() << " players from " << filepath;
}
