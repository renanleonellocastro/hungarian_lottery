#include <gtest/gtest.h>
#include "integration_common.h"

#include <chrono>
#include <condition_variable>
#include <filesystem>
#include <mutex>
#include <regex>
#include <string>
#include <thread>

using namespace std::chrono;
namespace fs = std::filesystem;

#if defined(TEST_DATA_DIR)
    std::string g_testDataDir = TEST_DATA_DIR;
#else
    std::string g_testDataDir = "tests/data";
#endif

// A streambuf that tees output to the original buffer and accumulates a local
// copy so tests can wait for "READY" and for the result string.
class TeeBuf : public std::streambuf {
public:
    TeeBuf(std::streambuf* dest) : m_dest(dest) {}

    // Wait until the accumulated buffer contains substring sub or timeout.
    bool waitFor(const std::string& sub, milliseconds timeout, steady_clock::time_point& when) {
        std::unique_lock lock(m_mutex);
        auto pred = [&]{ return m_buffer.find(sub) != std::string::npos; };

        if (m_cv.wait_for(lock, timeout, pred)) {
            when = steady_clock::now();
            return true;
        }

        return false;
    }

    std::string getBuffer() {
        std::lock_guard lock(m_mutex);
        return m_buffer;
    }

protected:
    int_type overflow(int_type ch) override {
        if (ch == traits_type::eof()) {
            return traits_type::not_eof(ch);
        }

        char c = traits_type::to_char_type(ch);
        // write to original buffer
        m_dest->sputc(c);
        // append locally and notify
        m_mutex.lock();
        m_buffer.push_back(c);
        m_mutex.unlock();
        m_cv.notify_all();
        return ch;
    }

    int sync() override {
        return m_dest->pubsync();
    }

private:
    std::streambuf* m_dest;
    std::string m_buffer;
    std::mutex m_mutex;
    std::condition_variable m_cv;
};

// A blocking input streambuf: when underflow is called and no data is
// available, it waits until data is pushed via `push()`.
class BlockingInputBuf : public std::streambuf {
public:
    BlockingInputBuf() {
        m_internal.reserve(g_bufSize);
        setg(nullptr, nullptr, nullptr);
    }

    void pushData(const std::string& sub) {
        m_mutex.lock();

        for (char c : sub) {
            m_buf.push_back(c);
        }

        m_mutex.unlock();
        m_cv.notify_one();
    }

protected:
    int_type underflow() override {
        std::unique_lock lock(m_mutex);
        m_cv.wait(lock, [&]{ return !m_buf.empty(); });

        // Move up to g_bufSize bytes into internal buffer
        size_t n = std::min(m_buf.size(), static_cast<size_t>(g_bufSize));
        m_internal.assign(m_buf.begin(), m_buf.begin() + n);
        m_buf.erase(m_buf.begin(), m_buf.begin() + n);

        setg(m_internal.data(), m_internal.data(), m_internal.data() + m_internal.size());
        return traits_type::to_int_type(*gptr());
    }

private:
    static constexpr size_t g_bufSize = 4096;
    std::vector<char> m_buf;
    std::vector<char> m_internal;
    std::mutex m_mutex;
    std::condition_variable m_cv;
};

// Execute one integration run for the provided input index and mode.
static void runIntegrationTestForIdxAndMode(int idx, LotteryMode mode) {
    std::string dataFile = g_testDataDir + "/sample_input_file_" + std::to_string(idx) + ".txt";
    std::string expectedFile = g_testDataDir + "/sample_expected_" + std::to_string(idx) + ".txt";

    // read expected line
    std::ifstream efs(expectedFile);
    ASSERT_TRUE(efs.is_open());
    std::string expectedLine;
    std::getline(efs, expectedLine);
    efs.close();

    // prepare argv
    char arg0[] = "lottery";
    std::vector<char> arg1buf(1024);
    strncpy(arg1buf.data(), dataFile.c_str(), arg1buf.size());
    char* argv[] = { arg0, arg1buf.data() };

    // Prepare blocking stdin so the application will wait until we push picks.
    static thread_local BlockingInputBuf inputBuf;
    auto* oldin = std::cin.rdbuf(&inputBuf);

    // Tee std::cout so we can observe output in real time.
    auto* oldOutBuf = std::cout.rdbuf();
    TeeBuf tee(oldOutBuf);
    std::cout.rdbuf(&tee);

    Lottery lottery;

    // Run the application in a separate thread so we can wait for READY and then inject stdin.
    auto threadStartTime = steady_clock::now();
    std::thread t([&]{
        try {
            lottery.initialize(2, argv, mode);
            lottery.run();
        } catch (...) {
        }
    });

    // Time until READY
    steady_clock::time_point readyTime;
    bool readyStringFound = tee.waitFor("READY", seconds(30), readyTime);
    ASSERT_TRUE(readyStringFound) << "Timed out waiting for READY for index " << idx;

    // Calculate time to READY
    auto microsecondsFromStartUntilReady = duration_cast<microseconds>(readyTime - threadStartTime).count();

    // Inject picks into stdin now that READY appeared.
    std::string picks = "1 2 3 4 5\n";
    inputBuf.pushData(picks);

    // Wait for expectedLine to appear in output
    steady_clock::time_point resultTime;
    bool resultStringFound = tee.waitFor(expectedLine, seconds(30), resultTime);
    ASSERT_TRUE(resultStringFound) << "Timed out waiting for expected result for index " << idx;

    // Print timings to stdout (these will be captured by the test runner)
    auto totalElapsedTime = duration_cast<microseconds>(resultTime - threadStartTime).count();
    std::cout << "INTEGRATION TEST: " << idx << " \033[32mPASSED" << "\033[0m" << std::endl;
    std::cout << "PLAYERS: " << lottery.getNumberOfPlayers() << std::endl;
    std::cout << "MODE: " << (mode==LotteryMode::BruteForce ? "BruteForce" : "Cached") << std::endl;
    std::cout << "READY: " << microsecondsFromStartUntilReady << "us" << std::endl;
    std::cout << "RESULT: " << (totalElapsedTime - microsecondsFromStartUntilReady) << "us" << std::endl;
    std::cout << "TOTAL: " << totalElapsedTime << "us" << std::endl << std::endl;

    // Verify expectedLine present
    std::string out = tee.getBuffer();
    EXPECT_NE(out.find(expectedLine), std::string::npos) << "Output did not contain expected for index " << idx;

    // restore streams
    std::cout.rdbuf(oldOutBuf);
    std::cin.rdbuf(oldin);

    if (t.joinable()) {
        t.join();
    }
}

TEST(Integration01, FullRun) {
    std::vector<LotteryMode> modes = { LotteryMode::Cached };
    for (int i = 1; i <= 100; ++i) {
        for (auto mode : modes) {
            runIntegrationTestForIdxAndMode(i, mode);
        }
    }
}
