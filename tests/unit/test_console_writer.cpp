#include <gtest/gtest.h>
#include <sstream>
#include <iostream>

#include "console_writer.h"
#include "winners.h"

using namespace hungarian;

TEST(ConsoleWriter, HelpAndReady) {
    std::ostringstream oss;
    auto* old = std::cout.rdbuf(oss.rdbuf());

    ConsoleWriter::printHelpMessage();
    ConsoleWriter::printReady();

    std::cout.rdbuf(old);
    std::string out = oss.str();
    EXPECT_NE(out.find("Usage: lottery"), std::string::npos);
    EXPECT_NE(out.find("READY"), std::string::npos);
}

TEST(ConsoleWriter, PrintResult) {
    std::ostringstream oss;
    auto* old = std::cout.rdbuf(oss.rdbuf());

    Winners w{};
    w.two = 1; w.three = 2; w.four = 3; w.five = 4;
    ConsoleWriter::printResult(w);

    std::cout.rdbuf(old);
    std::string out = oss.str();
    EXPECT_NE(out.find("1 2 3 4"), std::string::npos);
}
