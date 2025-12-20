#include <gtest/gtest.h>
#include <fstream>
#include <cstdio>

#include "players_numbers_reader.h"

using namespace hungarian;

static std::string make_tmp_file(const std::string& content) {
    std::string path = "tests_tmp_players.txt";
    std::ofstream ofs(path);
    ofs << content;
    ofs.close();
    return path;
}

TEST(PlayersNumbersReader, ReadValidFile) {
    std::string content = "1 2 3 4 5\n6 7 8 9 10\n";
    std::string path = make_tmp_file(content);

    PlayersNumbersReader r;
    EXPECT_NO_THROW(r.readFile(path));
    const auto& p = r.players();
    EXPECT_EQ(p.size(), 2);
    EXPECT_EQ(p[0].size(), 5);

    std::remove(path.c_str());
}

TEST(PlayersNumbersReader, FileNotFound) {
    PlayersNumbersReader r;
    EXPECT_THROW(r.readFile("nonexistent_file.txt"), std::runtime_error);
}

TEST(PlayersNumbersReader, InvalidCount) {
    std::string content = "1 2 3 4\n"; // only 4 numbers
    std::string path = make_tmp_file(content);
    PlayersNumbersReader r;
    EXPECT_THROW(r.readFile(path), std::runtime_error);
    std::remove(path.c_str());
}

TEST(PlayersNumbersReader, DuplicateNumbers) {
    std::string content = "1 2 3 3 5\n"; // duplicate 3
    std::string path = make_tmp_file(content);
    PlayersNumbersReader r;
    EXPECT_THROW(r.readFile(path), std::runtime_error);
    std::remove(path.c_str());
}

TEST(PlayersNumbersReader, OutOfRange) {
    std::string content = "1 2 3 4 91\n"; // 91 out of range
    std::string path = make_tmp_file(content);
    PlayersNumbersReader r;
    EXPECT_THROW(r.readFile(path), std::runtime_error);
    std::remove(path.c_str());
}
