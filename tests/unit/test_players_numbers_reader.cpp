#include <gtest/gtest.h>
#include <fstream>
#include <cstdio>

#include "players_numbers_reader.h"

using namespace hungarian;
class TempFile {
public:
    explicit TempFile(const std::string& content) {
        path = "players_numbers_test.tmp";
        std::ofstream(path) << content;
    }

    ~TempFile() {
        std::remove(path.c_str());
    }

    const std::string& get() const { return path; }

private:
    std::string path;
};

TEST(PlayersNumbersReaderTest, ReadsValidFile) {
    TempFile file("1 2 3 4 5\n6 7 8 9 10\n");

    PlayersNumbersReader reader;
    EXPECT_NO_THROW(reader.readFile(file.get()));

    const auto& players = reader.players();
    EXPECT_EQ(players.size(), 2);
    EXPECT_EQ(players[0][0], 1);
    EXPECT_EQ(players[1][4], 10);
}

TEST(PlayersNumbersReaderTest, EmptyFileProducesNoPlayers) {
    TempFile file("");

    PlayersNumbersReader reader;
    EXPECT_NO_THROW(reader.readFile(file.get()));
    EXPECT_TRUE(reader.players().empty());
}

TEST(PlayersNumbersReaderTest, EmptyLinesAreIgnored) {
    TempFile file("\n\n\n");

    PlayersNumbersReader reader;
    EXPECT_NO_THROW(reader.readFile(file.get()));
    EXPECT_TRUE(reader.players().empty());
}

TEST(PlayersNumbersReaderTest, ThrowsIfFileDoesNotExist) {
    PlayersNumbersReader reader;
    EXPECT_THROW(reader.readFile("no_such_file.txt"), std::runtime_error);
}

TEST(PlayersNumbersReaderTest, ThrowsOnTooFewNumbers) {
    TempFile file("1 2 3 4\n");

    PlayersNumbersReader reader;
    EXPECT_THROW(reader.readFile(file.get()), std::runtime_error);
}

TEST(PlayersNumbersReaderTest, ThrowsOnTooManyNumbers) {
    TempFile file("1 2 3 4 5 6\n");

    PlayersNumbersReader reader;
    EXPECT_THROW(reader.readFile(file.get()), std::runtime_error);
}

TEST(PlayersNumbersReaderTest, ThrowsOnDuplicateNumbers) {
    TempFile file("1 2 3 3 5\n");

    PlayersNumbersReader reader;
    EXPECT_THROW(reader.readFile(file.get()), std::runtime_error);
}

TEST(PlayersNumbersReaderTest, ThrowsOnValueBelowRange) {
    TempFile file("0 2 3 4 5\n");

    PlayersNumbersReader reader;
    EXPECT_THROW(reader.readFile(file.get()), std::runtime_error);
}

TEST(PlayersNumbersReaderTest, ThrowsOnValueAboveRange) {
    TempFile file("1 2 3 4 91\n");

    PlayersNumbersReader reader;
    EXPECT_THROW(reader.readFile(file.get()), std::runtime_error);
}

TEST(PlayersNumbersReaderTest, ThrowsOnNonNumericInput) {
    TempFile file("1 2 x 4 5\n");

    PlayersNumbersReader reader;
    EXPECT_THROW(reader.readFile(file.get()), std::runtime_error);
}

TEST(PlayersNumbersReaderTest, PreservesOrderAcrossThreads) {
    std::ostringstream oss;
    for (int i = 0; i < 1000; ++i) {
        int between1and90 = (i % 86) + 1;
        oss << between1and90 << " " << between1and90+1 << " " << between1and90+2 << " " << between1and90+3 << " " << between1and90+4 << "\n";
    }

    TempFile file(oss.str());

    PlayersNumbersReader reader;
    EXPECT_NO_THROW(reader.readFile(file.get()));

    const auto& players = reader.players();
    EXPECT_EQ(players.size(), 1000);
    EXPECT_EQ(players.front()[0], 1);
    EXPECT_EQ(players.back()[0], (999 % 86) + 1);
}

TEST(PlayersNumbersReaderTest, SingleLineUsesSingleThreadSafely) {
    TempFile file("1 2 3 4 5\n");

    PlayersNumbersReader reader;
    EXPECT_NO_THROW(reader.readFile(file.get()));
    EXPECT_EQ(reader.players().size(), 1);
}
