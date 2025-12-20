#include <iostream>
#include <array>
#include <exception>
#include "../include/logging_shim.h"

#include "players_numbers_reader.h"
#include "console_reader.h"
#include "console_writer.h"
#include "lottery_processor.h"

// forward factories
#include <memory>

namespace hungarian {
    std::unique_ptr<LotteryProcessor<std::vector<std::vector<uint8_t>>>> createBruteForceProcessor();
    std::unique_ptr<LotteryProcessor<std::vector<std::vector<uint8_t>>>> createCachedProcessor();
}

int main(int argc, char** argv) {
#ifdef HAVE_GLOG
    google::InitGoogleLogging(argv[0]);
    FLAGS_logtostderr = 1; // log to stderr
#else
    (void)argc; (void)argv;
#endif

    if (argc < 2) {
        std::cerr << "usage: " << argv[0] << " <players_file>\n";
        return 1;
    }

    try {
        hungarian::PlayersNumbersReader reader;
        reader.readFile(argv[1]);

        hungarian::ConsoleWriter::printReady();

        hungarian::ConsoleReader creader;
        creader.readLotteryPick();

        std::array<uint8_t,5> pick = {
            creader.m_lotteryPickFirstNumber,
            creader.m_lotteryPickSecondNumber,
            creader.m_lotteryPickThirdNumber,
            creader.m_lotteryPickFourthNumber,
            creader.m_lotteryPickFifthNumber
        };

        // choose processor - cached by default
        auto processor = hungarian::createCachedProcessor();
        processor->preprocess(reader.players());
        auto winners = processor->computeWinners(pick);
        hungarian::ConsoleWriter::printResult(winners.five, winners.four, winners.three, winners.two);
    } catch (const std::exception& ex) {
        LOG(FATAL) << "Unhandled exception: " << ex.what();
        return 2;
    }
    return 0;
}
