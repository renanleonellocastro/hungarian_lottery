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

static int players_count_for_index(int idx) {
    // idx in 1..100 -> linear from 1000 to 20,000,000
    const int min = 1000;
    const int max = 20000000;
    int step = (max - min) / 99;
    return min + (idx - 1) * step;
}

static void write_line(std::ofstream& ofs, const std::array<int,5>& arr) {
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

    write_line(ofs, arr);
    return true;
}

static void non_winner(std::ofstream& ofs, const std::array<int,5>& pick, int& next_val) {
    std::array<int,5> arr;

    for (int i=0;i<5;i++) {
        if (next_val == 91) {
            next_val = 6;
        }

        arr[i] = next_val;
        ++next_val;
    }

    write_line(ofs, arr);
}

static void generate_sample_file(int idx, int required_n, const std::string& data_path, const std::string& expected_path) {
    std::ofstream ofs(data_path, std::ios::trunc);
    // fixed pick
    const std::array<int,5> pick = {1,2,3,4,5};
    // counts
    Winners winners{};

    int written = 0;
    int next_val = 6;

    // fill with numbers
    while (written < required_n) {
        if (!winner(ofs, pick, winners)) {
            non_winner(ofs, pick, next_val);
        }

        ++written;
    }

    ofs.close();

    // write expected file: format "two three four five" as ConsoleWriter prints
    std::ofstream efs(expected_path, std::ios::trunc);
    efs << winners.two << ' ' << winners.three << ' ' << winners.four << ' ' << winners.five << '\n';
    efs.close();
}

static void generate_data_files(int idx) {
    int n = players_count_for_index(idx);
    std::string dir = "tests/data";
    std::string data_file = dir + "/sample_input_file_" + std::to_string(idx) + ".txt";
    std::string expected_file = dir + "/sample_expected_" + std::to_string(idx) + ".txt";

    // ensure data directory exists: tests will run from project root so create if needed
    system((std::string("mkdir -p ") + dir).c_str());

    EXPECT_NO_THROW(generate_sample_file(idx, n, data_file, expected_file));
}
