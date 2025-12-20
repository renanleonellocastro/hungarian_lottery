#include "lottery.h"
#include "brute_force_lottery_processor.h"

using namespace hungarian;

void Lottery::run(int argc, char** argv, LotteryMode mode) {
    // Parse arguments
    std::string playersNumbersFile = parseArguments(argc, argv);

    // Read players' numbers from file
    m_playersNumbersReader.readFile(playersNumbersFile);
    const auto& players = m_playersNumbersReader.players();

    // Initialize processor
    instantiateProcessor(mode);

    // Preprocess players
    m_processor->preComputeWinners(players);

    // Indicate readiness
    ConsoleWriter::printReady();

    // Read lottery pick from stdin
    m_consoleReader.readLotteryPick();
    std::array<uint8_t,5> lotteryPick = m_consoleReader.getLotteryPick();

    // Compute winners
    Winners winners = m_processor->computeWinners(lotteryPick);

    // Print results
    ConsoleWriter::printResult(winners);
}

std::uint32_t Lottery::getNumberOfPlayers() {
    return m_playersNumbersReader.players().size();
}

std::string Lottery::parseArguments(int argc, char** argv) const {
    if (argc != 2) {
        ConsoleWriter::printHelpMessage();
        throw std::runtime_error("invalid arguments");
    }

    return std::string(argv[1]);
}

void Lottery::instantiateProcessor(LotteryMode mode) {
    switch (mode) {
        case LotteryMode::BruteForce:
            m_processor = std::make_unique<BruteForceLotteryProcessor>();
            break;
        default:
            throw std::runtime_error("Unknown lottery mode");
    }
}
