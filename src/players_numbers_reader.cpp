#include "players_numbers_reader.h"

#include <thread>
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

    validateOnlyNumbers(lineStream, lineNumber);

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

void PlayersNumbersReader::validateOnlyNumbers(const std::istringstream& lineStream, size_t lineNumber) const {
    if (!lineStream.eof()) {
        LOG(ERROR) << "Invalid characters on line " << lineNumber;
        throw std::runtime_error("invalid characters");
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
    auto lines = readLinesFromFile(filepath);
    auto threadCount = determineThreadCount(lines.size());
    auto chunks = parseLinesInParallel(lines, threadCount);
    mergeParsedChunks(chunks, lines.size());

    LOG(INFO) << "Read " << m_players.size() << " players from " << filepath << " using " << threadCount << " threads";
}

std::vector<std::string> PlayersNumbersReader::readLinesFromFile(const std::string& filepath) const {
    std::ifstream file(filepath);

    if (!file) {
        LOG(ERROR) << "Failed to open input file: " << filepath;
        throw std::runtime_error("failed to open input file");
    }

    std::vector<std::string> lines;
    std::string line;

    while (std::getline(file, line)) {
        if (!line.empty()) {
            lines.emplace_back(std::move(line));
        }
    }

    return lines;
}

size_t PlayersNumbersReader::determineThreadCount(size_t workSize) const {
    if (workSize == 0) {
        return 0;
    }

    const auto hw = std::thread::hardware_concurrency();
    const size_t threads = hw == 0 ? 1 : hw;

    return std::min(threads, workSize);
}

std::vector<std::vector<std::array<uint8_t,5>>> PlayersNumbersReader::parseLinesInParallel(const std::vector<std::string>& lines, size_t threadCount) const {
    const size_t chunkSize = computeChunkSize(lines.size(), threadCount);
    std::vector<std::future<std::vector<std::array<uint8_t,5>>>> tasks;

    for (size_t i = 0; i < threadCount; ++i) {
        tasks.emplace_back(spawnParseTask(lines, i, chunkSize));
    }

    return collectTaskResults(tasks);
}

size_t PlayersNumbersReader::computeChunkSize(size_t totalLines, size_t threadCount) const {
    if (threadCount == 0) {
        return 0;
    }

    return (totalLines + threadCount - 1) / threadCount;
}

std::future<std::vector<std::array<uint8_t,5>>> PlayersNumbersReader::spawnParseTask(const std::vector<std::string>& lines, size_t taskIndex, size_t chunkSize) const {
    return std::async(std::launch::async, [this, &lines, taskIndex, chunkSize] {
        return parseChunk(lines, taskIndex, chunkSize);
    });
}

std::vector<std::array<uint8_t,5>> PlayersNumbersReader::parseChunk(const std::vector<std::string>& lines, size_t taskIndex, size_t chunkSize) const {
    // Rounding the chunk size up can leave trailing tasks without lines, so clamp both bounds.
    const size_t begin = std::min(taskIndex * chunkSize, lines.size());
    const size_t end = std::min(begin + chunkSize, lines.size());

    std::vector<std::array<uint8_t,5>> parsed;
    parsed.reserve(end - begin);

    for (size_t i = begin; i < end; ++i) {
        parsed.push_back(parseLine(lines[i], i + 1));
    }

    return parsed;
}

std::vector<std::vector<std::array<uint8_t,5>>> PlayersNumbersReader::collectTaskResults(std::vector<std::future<std::vector<std::array<uint8_t,5>>>>& tasks) const {
    std::vector<std::vector<std::array<uint8_t,5>>> results;
    results.reserve(tasks.size());

    for (auto& task : tasks) {
        results.emplace_back(task.get());
    }

    return results;
}

void PlayersNumbersReader::mergeParsedChunks(std::vector<std::vector<std::array<uint8_t,5>>>& chunks, size_t totalLines) {
    m_players.clear();
    m_players.reserve(totalLines);

    for (auto& chunk : chunks) {
        m_players.insert(m_players.end(), std::make_move_iterator(chunk.begin()), std::make_move_iterator(chunk.end()));
    }
}
