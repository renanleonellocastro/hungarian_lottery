/**
 * @file lottery_benchmark.cpp
 * @brief Measures load, preprocessing and per draw latency of every lottery strategy.
 *
 * Usage: `lottery_benchmark players_file draws_file [repetitions]`
 *
 * The draws file uses the players file format (five numbers per line). Every
 * draw is evaluated `repetitions` times per strategy and the latency
 * distribution is printed as a Markdown table row. The tool exits with a non
 * zero status if the strategies disagree on any draw.
 */
#include <glog/logging.h>

#include <array>
#include <chrono>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <iostream>
#include <algorithm>

#include "players_numbers_reader.h"
#include "cached_lottery_processor.h"
#include "brute_force_lottery_processor.h"

using namespace hungarian;
using Clock = std::chrono::steady_clock;
using Ticket = std::array<uint8_t,5>;

namespace {

struct Arguments {
    std::string playersFile;
    std::string drawsFile;
    int repetitions = 5;
};

struct Strategy {
    std::string name;
    std::unique_ptr<ILotteryProcessor> processor;
};

struct Measurement {
    double precomputeMs = 0;
    std::vector<double> drawLatenciesUs;
    std::vector<Winners> results;
};

double millisecondsSince(Clock::time_point start) {
    return std::chrono::duration<double, std::milli>(Clock::now() - start).count();
}

double microsecondsSince(Clock::time_point start) {
    return std::chrono::duration<double, std::micro>(Clock::now() - start).count();
}

double percentile(std::vector<double> values, double fraction) {
    std::sort(values.begin(), values.end());
    const auto index = static_cast<size_t>(fraction * static_cast<double>(values.size() - 1));
    return values[index];
}

std::vector<Ticket> readTickets(const std::string& path) {
    PlayersNumbersReader reader;
    reader.readFile(path);
    return reader.players();
}

std::vector<Strategy> makeStrategies() {
    std::vector<Strategy> strategies;
    strategies.push_back({"BruteForce", std::make_unique<BruteForceLotteryProcessor>()});
    strategies.push_back({"Cached", std::make_unique<CachedLotteryProcessor>()});
    return strategies;
}

Measurement measure(ILotteryProcessor& processor, const std::vector<Ticket>& players, const std::vector<Ticket>& draws, int repetitions) {
    Measurement measurement;

    const auto precomputeStart = Clock::now();
    processor.preComputeWinners(players);
    measurement.precomputeMs = millisecondsSince(precomputeStart);

    for (int repetition = 0; repetition < repetitions; ++repetition) {
        for (const auto& draw : draws) {
            const auto drawStart = Clock::now();
            const Winners winners = processor.computeWinners(draw);
            measurement.drawLatenciesUs.push_back(microsecondsSince(drawStart));

            if (repetition == 0) {
                measurement.results.push_back(winners);
            }
        }
    }

    return measurement;
}

bool sameResults(const std::vector<Winners>& left, const std::vector<Winners>& right) {
    return std::equal(left.begin(), left.end(), right.begin(), right.end(), [](const Winners& a, const Winners& b) {
        return a.two == b.two && a.three == b.three && a.four == b.four && a.five == b.five;
    });
}

void printHeader() {
    std::cout << "| Strategy | Players | Load (ms) | Preprocess (ms) | Draw p50 (us) | Draw p99 (us) | Draw max (us) |\n"
              << "|---|---:|---:|---:|---:|---:|---:|\n";
}

void printRow(const std::string& name, size_t players, double loadMs, const Measurement& measurement) {
    const auto& latencies = measurement.drawLatenciesUs;
    std::cout << std::fixed << std::setprecision(1)
              << "| " << name << " | " << players << " | " << loadMs << " | " << measurement.precomputeMs
              << " | " << percentile(latencies, 0.50) << " | " << percentile(latencies, 0.99)
              << " | " << *std::max_element(latencies.begin(), latencies.end()) << " |\n";
}

Arguments parseArguments(int argc, char** argv) {
    if (argc < 3 || argc > 4) {
        throw std::runtime_error("usage: lottery_benchmark <players_file> <draws_file> [repetitions]");
    }

    Arguments arguments{argv[1], argv[2]};

    if (argc == 4) {
        arguments.repetitions = std::max(1, std::stoi(argv[3]));
    }

    return arguments;
}

int runBenchmark(const Arguments& arguments) {
    const auto loadStart = Clock::now();
    const auto players = readTickets(arguments.playersFile);
    const double loadMs = millisecondsSince(loadStart);
    const auto draws = readTickets(arguments.drawsFile);

    if (draws.empty()) {
        throw std::runtime_error("the draws file is empty");
    }

    std::vector<Winners> reference;
    printHeader();

    for (auto& strategy : makeStrategies()) {
        const auto measurement = measure(*strategy.processor, players, draws, arguments.repetitions);
        printRow(strategy.name, players.size(), loadMs, measurement);

        if (reference.empty()) {
            reference = measurement.results;
        } else if (!sameResults(reference, measurement.results)) {
            std::cerr << "Strategies disagree on at least one draw\n";
            return 1;
        }
    }

    return 0;
}

} // namespace

int main(int argc, char** argv) {
    google::InitGoogleLogging(argv[0]);
    FLAGS_logtostderr = 1;
    FLAGS_minloglevel = google::GLOG_ERROR;

    try {
        return runBenchmark(parseArguments(argc, argv));
    } catch (const std::exception& ex) {
        std::cerr << ex.what() << '\n';
        return 1;
    }
}
