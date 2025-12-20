#include <gtest/gtest.h>
#include <sstream>

#include "console_reader.h"

using namespace hungarian;

TEST(ConsoleReader, ReadValidPick) {
    std::istringstream iss("1 2 3 4 5\n");
    auto* old = std::cin.rdbuf(iss.rdbuf());

    ConsoleReader r;
    EXPECT_NO_THROW(r.readLotteryPick());
    auto pick = r.getLotteryPick();
    EXPECT_EQ(pick[0], 1);
    EXPECT_EQ(pick[4], 5);

    std::cin.rdbuf(old);
}

TEST(ConsoleReader, ReadInvalidPick) {
    std::istringstream iss("1 2 3 400 5\n");
    auto* old = std::cin.rdbuf(iss.rdbuf());

    ConsoleReader r;
    EXPECT_THROW(r.readLotteryPick(), std::runtime_error);

    std::cin.rdbuf(old);
}
