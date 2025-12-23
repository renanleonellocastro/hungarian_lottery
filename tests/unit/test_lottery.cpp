#include <gtest/gtest.h>
#include <fstream>
#include <cstdio>
#include <sstream>

#include "lottery.h"

using namespace hungarian;

static std::string makeTmpFile(const std::string& content) {
    std::string path = "tests_tmp_players_lottery.txt";
    std::ofstream ofs(path);
    ofs << content;
    ofs.close();
    return path;
}

TEST(LotteryRun, EndToEndBruteForce) {
    std::string content = "1 2 3 4 5\n6 7 8 9 10\n";
    std::string path = makeTmpFile(content);

    // Prepare argv
    char arg0[] = "prog";
    char arg1[256];
    strncpy(arg1, path.c_str(), sizeof(arg1));
    char* argv[] = { arg0, arg1 };

    // Prepare stdin
    std::istringstream iss("1 2 3 4 5\n");
    auto* oldin = std::cin.rdbuf(iss.rdbuf());

    // Capture stdout
    std::ostringstream oss;
    auto* oldout = std::cout.rdbuf(oss.rdbuf());

    Lottery lottery;
    EXPECT_NO_THROW(lottery.initialize(2, argv, LotteryMode::BruteForce));
    EXPECT_NO_THROW(lottery.run());

    std::cout.rdbuf(oldout);
    std::cin.rdbuf(oldin);

    std::string out = oss.str();
    EXPECT_NE(out.find("READY"), std::string::npos);
    EXPECT_NE(out.find("0 0 0 1"), std::string::npos);

    std::remove(path.c_str());
}
