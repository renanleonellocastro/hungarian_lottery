#include <gtest/gtest.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include "lottery.h"

using namespace hungarian;

TEST(LotteryTargeted, InitializeInvalidArgsThrows) {
    Lottery l;
    char* argv[] = { (char*)"prog", nullptr };
    EXPECT_THROW(l.initialize(1, argv, LotteryMode::BruteForce), std::runtime_error);
}

TEST(LotteryTargeted, InstantiateProcessorDefaultThrows) {
    Lottery l;

    // create an empty temp file to act as players file
    const std::string path = "test_players_tmp.txt";
    {
        std::ofstream ofs(path);
        ofs << "";
    }

    const char* args[] = { "prog", path.c_str() };
    char* argv[] = { const_cast<char*>(args[0]), const_cast<char*>(args[1]), nullptr };

    EXPECT_THROW(l.initialize(2, argv, static_cast<LotteryMode>(999)), std::runtime_error);

    // cleanup
    std::remove(path.c_str());
}

TEST(LotteryTargeted, RunBruteForceHappyPathReadsPlayersAndPick) {
    Lottery l;

    // create a players file with one matching player
    const std::string path = "test_players_one.txt";
    {
        std::ofstream ofs(path);
        ofs << "1 2 3 4 5\n";
    }

    const char* args[] = { "prog", path.c_str() };
    char* argv[] = { const_cast<char*>(args[0]), const_cast<char*>(args[1]), nullptr };

    // prepare stdin with the exact pick
    std::istringstream input("1 2 3 4 5\n");
    auto* old = std::cin.rdbuf(input.rdbuf());

    EXPECT_NO_THROW(l.initialize(2, argv, LotteryMode::BruteForce));
    EXPECT_NO_THROW(l.run());
    std::cin.rdbuf(old);

    EXPECT_EQ(l.getNumberOfPlayers(), 1u);

    // cleanup
    std::remove(path.c_str());
}

TEST(LotteryTargeted, RunCachedPathConstructsProcessor) {
    Lottery l;

    // empty players file (no players)
    const std::string path = "test_players_empty_cached.txt";
    {
        std::ofstream ofs(path);
        ofs << "";
    }

    const char* args[] = { "prog", path.c_str() };
    char* argv[] = { const_cast<char*>(args[0]), const_cast<char*>(args[1]), nullptr };

    // provide a valid pick on stdin to avoid blocking
    std::istringstream input("1 2 3 4 5\n");
    auto* old = std::cin.rdbuf(input.rdbuf());

    EXPECT_NO_THROW(l.initialize(2, argv, LotteryMode::Cached));
    EXPECT_NO_THROW(l.run());

    std::cin.rdbuf(old);
    std::remove(path.c_str());
}
