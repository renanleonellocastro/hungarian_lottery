#pragma once

#include <array>
#include <string>
#include <vector>
#include <cstdio>
#include <random>
#include <fstream>
#include <sstream>
#include <iostream>

#include "lottery.h"

using namespace hungarian;

static int playersCountForIndex(int idx) {
    // idx in 1..100 -> linear from 1000 to 20,000,000
    const int min = 1000;
    const int max = 20000000;
    int step = (max - min) / 99;
    return min + (idx - 1) * step;
}

static void writeLine(std::ofstream& ofs, const std::array<int,5>& arr) {
    ofs << arr[0] << ' ' << arr[1] << ' ' << arr[2] << ' ' << arr[3] << ' ' << arr[4] << '\n';
}

static bool lucky(float percentage) {
    if (percentage <= 0.0f) {
       return false;
    }

    if (percentage >= 100.0f) {
       return true;
    }

    static thread_local std::mt19937 generator(std::random_device{}());
    std::uniform_real_distribution<float> distribution(0.0f, 100.0f);
    return distribution(generator) < percentage;
}

static bool winner(std::ofstream& ofs, const std::array<int,5>& pick, Winners& winners) {
    std::array<int,5> arr;

    if (lucky(0.001f)) {
        arr = pick;
        ++winners.five;
    } else if (lucky(0.01f)) {
        arr = pick;
        arr[4] = 6;
        ++winners.four;
    } else if (lucky(0.05f)) {
        arr = pick;
        arr[3] = 6;
        arr[4] = 7;
        ++winners.three;
    } else if (lucky(0.1f)) {
        arr = pick;
        arr[2] = 6;
        arr[3] = 7;
        arr[4] = 8;
        ++winners.two;
    } else {
        return false;
    }

    writeLine(ofs, arr);
    return true;
}

static void looser(std::ofstream& ofs, const std::array<int,5>& pick, int& nextVal) {
    std::array<int,5> arr;

    for (int i=0;i<5;i++) {
        if (nextVal == 91) {
            nextVal = 6;
        }

        arr[i] = nextVal;
        ++nextVal;
    }

    writeLine(ofs, arr);
}

static void generateSampleFile(int idx, int playersCount, const std::string& dataPath, const std::string& expectedPath) {
    std::ofstream ofs(dataPath, std::ios::trunc);
    // fixed pick
    const std::array<int,5> pick = {1,2,3,4,5};
    // counts
    Winners winners{};

    int written = 0;
    int nextVal = 6;

    // fill with numbers
    while (written < playersCount) {
        if (!winner(ofs, pick, winners)) {
            looser(ofs, pick, nextVal);
        }

        ++written;
    }

    ofs.close();

    // write expected file: format "two three four five" as ConsoleWriter prints
    std::ofstream efs(expectedPath, std::ios::trunc);
    efs << winners.two << ' ' << winners.three << ' ' << winners.four << ' ' << winners.five << '\n';
    efs.close();
}

static void generateDataFiles(int idx) {
    int playersCount = playersCountForIndex(idx);
    std::string dir = "tests/data";
    std::string dataFile = dir + "/sample_input_file_" + std::to_string(idx) + ".txt";
    std::string expectedFile = dir + "/sample_expected_" + std::to_string(idx) + ".txt";

    // ensure data directory exists: tests will run from project root so create if needed
    system((std::string("mkdir -p ") + dir).c_str());

    EXPECT_NO_THROW(generateSampleFile(idx, playersCount, dataFile, expectedFile));
}
