#include <gtest/gtest.h>

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>

#include "lottery.h"

using namespace hungarian;

#if defined(TEST_DATA_DIR)
static const std::string g_testDataDir = TEST_DATA_DIR;
#else
static const std::string g_testDataDir = "tests/data";
#endif

/**
 * @brief Redirects std::cin and std::cout for the lifetime of the object.
 */
class ConsoleRedirect {
public:
    ConsoleRedirect(std::istream& input, std::ostream& output)
        : m_oldIn(std::cin.rdbuf(input.rdbuf())), m_oldOut(std::cout.rdbuf(output.rdbuf())) {}

    ~ConsoleRedirect() {
        std::cin.rdbuf(m_oldIn);
        std::cout.rdbuf(m_oldOut);
    }

    ConsoleRedirect(const ConsoleRedirect&) = delete;
    ConsoleRedirect& operator=(const ConsoleRedirect&) = delete;

private:
    std::streambuf* m_oldIn;
    std::streambuf* m_oldOut;
};

static std::string readWholeFile(const std::string& path) {
    std::ifstream file(path);

    if (!file) {
        throw std::runtime_error("missing dataset file: " + path);
    }

    std::ostringstream content;
    content << file.rdbuf();
    return content.str();
}

/**
 * @brief Run the application over a generated dataset and return everything printed to stdout.
 */
static std::string runLottery(const std::string& datasetDir, LotteryMode mode) {
    std::string playersFile = datasetDir + "/players.txt";
    std::vector<char*> argv = { const_cast<char*>("lottery"), playersFile.data() };

    std::ifstream draws(datasetDir + "/draws.txt");
    std::ostringstream output;
    ConsoleRedirect redirect(draws, output);

    Lottery lottery;
    lottery.initialize(static_cast<int>(argv.size()), argv.data(), mode);

    while (lottery.run()) {
    }

    return output.str();
}

struct EndToEndCase {
    std::string dataset;
    LotteryMode mode;
};

static std::string caseName(const testing::TestParamInfo<EndToEndCase>& info) {
    const char* mode = info.param.mode == LotteryMode::Cached ? "Cached" : "BruteForce";
    return info.param.dataset + "_" + mode;
}

class EndToEnd : public testing::TestWithParam<EndToEndCase> {};

TEST_P(EndToEnd, OutputMatchesBruteForceOracle) {
    const std::string datasetDir = g_testDataDir + "/" + GetParam().dataset;
    const std::string expected = "READY\n" + readWholeFile(datasetDir + "/expected.txt");

    EXPECT_EQ(runLottery(datasetDir, GetParam().mode), expected);
}

INSTANTIATE_TEST_SUITE_P(GeneratedDatasets, EndToEnd, testing::Values(
    EndToEndCase{"small", LotteryMode::BruteForce},
    EndToEndCase{"small", LotteryMode::Cached},
    EndToEndCase{"medium", LotteryMode::BruteForce},
    EndToEndCase{"medium", LotteryMode::Cached}
), caseName);
