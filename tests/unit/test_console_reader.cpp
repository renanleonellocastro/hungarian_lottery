#include <gtest/gtest.h>
#include <sstream>

#include "console_reader.h"

using namespace hungarian;

TEST(ConsoleReader, ReadValidPick) {
    std::istringstream iss("1 2 3 4 5\n");
    auto* old = std::cin.rdbuf(iss.rdbuf());

    ConsoleReader consoleReader;
    EXPECT_NO_THROW(consoleReader.readLotteryPick());
    auto pick = consoleReader.getLotteryPick();
    EXPECT_EQ(pick[0], 1);
    EXPECT_EQ(pick[4], 5);

    std::cin.rdbuf(old);
}

TEST(ConsoleReader, ReadInvalidPick1) {
    std::istringstream iss("1 2 3 400 5\n");
    auto* old = std::cin.rdbuf(iss.rdbuf());

    ConsoleReader consoleReader;
    EXPECT_THROW(consoleReader.readLotteryPick(), std::runtime_error);

    std::cin.rdbuf(old);
}

TEST(ConsoleReader, ReadInvalidPick2) {
    std::istringstream iss("1 2 3 4 5 6\n");
    auto* old = std::cin.rdbuf(iss.rdbuf());

    ConsoleReader consoleReader;
    EXPECT_THROW(consoleReader.readLotteryPick(), std::runtime_error);

    std::cin.rdbuf(old);
}

TEST(ConsoleReader, ReadInvalidPick3) {
    std::istringstream iss("1 2 3 4\n");
    auto* old = std::cin.rdbuf(iss.rdbuf());

    ConsoleReader consoleReader;
    EXPECT_THROW(consoleReader.readLotteryPick(), std::runtime_error);

    std::cin.rdbuf(old);
}

TEST(ConsoleReader, ReadInvalidPick4) {
    std::istringstream iss("1 2 3 4 x\n");
    auto* old = std::cin.rdbuf(iss.rdbuf());

    ConsoleReader consoleReader;
    EXPECT_THROW(consoleReader.readLotteryPick(), std::runtime_error);

    std::cin.rdbuf(old);
}

TEST(ConsoleReader, ReadInvalidPick5) {
    std::istringstream iss("1 2 3 -4 5\n");
    auto* old = std::cin.rdbuf(iss.rdbuf());

    ConsoleReader consoleReader;
    EXPECT_THROW(consoleReader.readLotteryPick(), std::runtime_error);

    std::cin.rdbuf(old);
}

TEST(ConsoleReader, ReadInvalidPick6) {
    std::istringstream iss("1 2 2 4 5\n");
    auto* old = std::cin.rdbuf(iss.rdbuf());

    ConsoleReader consoleReader;
    EXPECT_THROW(consoleReader.readLotteryPick(), std::runtime_error);

    std::cin.rdbuf(old);
}

TEST(ConsoleReader, ReadInvalidPick7) {
    std::istringstream iss("256 2 3 4 5\n");
    auto* old = std::cin.rdbuf(iss.rdbuf());

    ConsoleReader consoleReader;
    EXPECT_THROW(consoleReader.readLotteryPick(), std::runtime_error);

    std::cin.rdbuf(old);
}

TEST(ConsoleReader, ReadInvalidPick8) {
    std::istringstream iss("1,2,3,4,5\n");
    auto* old = std::cin.rdbuf(iss.rdbuf());

    ConsoleReader consoleReader;
    EXPECT_THROW(consoleReader.readLotteryPick(), std::runtime_error);

    std::cin.rdbuf(old);
}

TEST(ConsoleReader, ReadInvalidPick9) {
    std::istringstream iss(" \n");
    auto* old = std::cin.rdbuf(iss.rdbuf());

    ConsoleReader consoleReader;
    EXPECT_THROW(consoleReader.readLotteryPick(), std::runtime_error);

    std::cin.rdbuf(old);
}

TEST(ConsoleReader, ReadInvalidPick10) {
    std::istringstream iss("1 2 three 4 5\n");
    auto* old = std::cin.rdbuf(iss.rdbuf());

    ConsoleReader consoleReader;
    EXPECT_THROW(consoleReader.readLotteryPick(), std::runtime_error);

    std::cin.rdbuf(old);
}