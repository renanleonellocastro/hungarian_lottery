#include "brute_force_lottery_processor.h"

using namespace hungarian;


void BruteForceLotteryProcessor::preComputeWinners(const std::vector<std::array<uint8_t,5>>& players) {
    m_container = players;
}

Winners BruteForceLotteryProcessor::computeWinners(const std::array<uint8_t,5>& pick) const {
    Winners w{};

    for (const auto& player : m_container) {
        int matches = countMatches(pick, player);
        computeWinner(matches, w);
    }

    return w;
}

int BruteForceLotteryProcessor::countMatches(const std::array<uint8_t,5>& pick, const std::array<uint8_t,5>& player) {
    int matches = 0;

    for (uint8_t num : player) {
        if (std::find(pick.begin(), pick.end(), num) != pick.end()) {
            ++matches;
        }
    }

    return matches;
}

void hungarian::BruteForceLotteryProcessor::computeWinner(int matches, Winners &w) const
{
    switch (matches) {
        case 5:
            ++w.five;
            break;
        case 4:
            ++w.four;
            break;
        case 3:
            ++w.three;
            break;
        case 2:
            ++w.two;
            break;
    }
}
