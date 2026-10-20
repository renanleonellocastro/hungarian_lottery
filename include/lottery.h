/**
 * @file lottery.h
 * @brief Hungarian Lottery App.
 */
#pragma once

#include "console_reader.h"
#include "console_writer.h"
#include "i_lottery_processor.h"
#include "players_numbers_reader.h"

namespace hungarian {

/**
 * @brief Lottery processing mode.
 * 
 * Defines the algorithm used for processing the lottery.
 */
enum class LotteryMode {
    BruteForce,
    Cached
};

/**
 * @brief Hungarian Lottery Application Class.
 *
 * This class is the higher-level interface for the Hungarian Lottery application.
 */
class Lottery {
public:
    /**
     * @brief Initialize a new Lottery with new players playing the game.
     * 
     * This initializes the lottery by reading players' numbers from the provided file path.
     * @param argc Argument count.
     * @param argv Argument vector.
     * @param mode Lottery processing mode.
     * @throws std::runtime_error on errors.
     */
    void initialize(int argc, char** argv, LotteryMode mode);

    /**
     * @brief Process one lottery draw: read it from stdin and print the winners.
     * @return true when a draw was processed, false when stdin reached end of input.
     * @throws std::runtime_error on errors.
     */
    bool run();

    /**
     * @brief Get the number of players loaded from the input file.
     * @return Number of players.
     */
    std::uint32_t getNumberOfPlayers();

private:
    /**
     * @brief ILottery processor instance which performs the core processing.
     * 
     * This is a polymorphic pointer to the interface ILotteryProcessor.
     */
    std::unique_ptr<ILotteryProcessor> m_processor;

    /**
     * @brief Console reader for input.
     */
    ConsoleReader m_consoleReader;

    /**
     * @brief Players numbers reader for reading players' selected numbers from file.
     */
    PlayersNumbersReader m_playersNumbersReader;

    /**
     * @brief Parse the input arguments.
     * 
     * This helper validates and extracts necessary parameters from the
     * command-line arguments. It prints a help message and throws on not valid arguments.
     * 
     * @return std::string Path to players numbers file.
     * @throws std::runtime_error on invalid arguments.
     */
    std::string parseArguments(int argc, char** argv) const;

    /**
     * @brief Instantiate the lottery processor based on the selected mode.
     * @param mode Lottery processing mode.
     */
    void instantiateProcessor(LotteryMode mode);
};

} // namespace hungarian
