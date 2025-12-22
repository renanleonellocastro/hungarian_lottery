/**
 * @file console_writer.h
 * @brief Helpers to write responses to stdout.
 */
#pragma once

#include <iostream>

#include "winners.h"

namespace hungarian {

class ConsoleWriter {
public:
    /**
     * @brief Deleted default constructor to prevent instantiation.
     */
    ConsoleWriter() = delete;

    /**
     * @brief Deleted destructor (class is static-only).
     */
    ~ConsoleWriter() = delete;

    /**
     * @brief Prints help message to stdout.
     */
    static void printHelpMessage();

    /**
     * @brief Prints READY and newline to stdout.
     */
    static void printReady();

    /**
     * @brief Prints the four result fields separated by spaces and newline to stdout.
     * @param winners Counts of winners for two, three, four, and five matches.
     */
    static void printResult(const Winners& winners);
};

} // namespace hungarian
