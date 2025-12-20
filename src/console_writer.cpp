#include "console_writer.h"

using namespace hungarian;

void ConsoleWriter::printHelpMessage() {
    std::cout << "Usage: lottery <players_numbers_file>\n";
}

void ConsoleWriter::printReady() {
    std::cout << "READY" << '\n';
}

void ConsoleWriter::printResult(const Winners& winners) {
    std::cout << winners.two << ' ' << winners.three << ' ' << winners.four << ' ' << winners.five << std::endl;
}
