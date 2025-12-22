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
    std::string g_test_data_dir = TEST_DATA_DIR;
#else
    std::string g_test_data_dir = "tests/data";
#endif

// A streambuf that tees output to the original buffer and accumulates a local
// copy so tests can wait for "READY" and for the result string.
class TeeBuf : public std::streambuf {
public:
    TeeBuf(std::streambuf* dest) : dest_(dest) {}

    // Wait until the accumulated buffer contains substring s or timeout.
    bool wait_for(const std::string& s, milliseconds timeout, steady_clock::time_point& when) {
        std::unique_lock lock(m_mutex);
        auto pred = [&]{ return buffer_.find(s) != std::string::npos; };

        if (cv_.wait_for(lock, timeout, pred)) {
            when = steady_clock::now();
            return true;
        }

        return false;
    }

    std::string str() {
        std::lock_guard lock(m_mutex);
        return buffer_;
    }

protected:
    int_type overflow(int_type ch) override {
        if (ch == traits_type::eof()) {
            return traits_type::not_eof(ch);
        }

        char c = traits_type::to_char_type(ch);
        // write to original buffer
        dest_->sputc(c);
        // append locally and notify
        m_mutex.lock();
        buffer_.push_back(c);
        m_mutex.unlock();
        cv_.notify_all();
        return ch;
    }

    int sync() override {
        return dest_->pubsync();
    }

private:
    std::streambuf* dest_;
    std::string buffer_;
    std::mutex m_mutex;
    std::condition_variable cv_;
};

// A blocking input streambuf: when underflow is called and no data is
// available, it waits until data is pushed via `push()`.
class BlockingInputBuf : public std::streambuf {
public:
    BlockingInputBuf() {
        internal_.reserve(kBufSize);
        setg(nullptr, nullptr, nullptr);
    }

    void push(const std::string& s) {
        m_mutex.lock();

        for (char c : s) {
            buf_.push_back(c);
        }

        m_mutex.unlock();
        cv_.notify_one();
    }

protected:
    int_type underflow() override {
        std::unique_lock lock(m_mutex);
        cv_.wait(lock, [&]{ return !buf_.empty(); });

        // Move up to kBufSize bytes into internal_ buffer
        size_t n = std::min(buf_.size(), static_cast<size_t>(kBufSize));
        internal_.assign(buf_.begin(), buf_.begin() + n);
        buf_.erase(buf_.begin(), buf_.begin() + n);

        setg(internal_.data(), internal_.data(), internal_.data() + internal_.size());
        return traits_type::to_int_type(*gptr());
    }

private:
    static constexpr size_t kBufSize = 4096;
    std::vector<char> buf_;
    std::vector<char> internal_;
    std::mutex m_mutex;
    std::condition_variable cv_;
};

// Execute one integration run for the provided input index and mode.
static void run_integration_test_for_idx_and_mode(int idx, LotteryMode mode) {
    std::string data_file = g_test_data_dir + "/sample_input_file_" + std::to_string(idx) + ".txt";
    std::string expected_file = g_test_data_dir + "/sample_expected_" + std::to_string(idx) + ".txt";

    // read expected line
    std::ifstream efs(expected_file);
    ASSERT_TRUE(efs.is_open());
    std::string expected_line;
    std::getline(efs, expected_line);
    efs.close();

    // prepare argv
    char arg0[] = "lottery";
    std::vector<char> arg1buf(1024);
    strncpy(arg1buf.data(), data_file.c_str(), arg1buf.size());
    char* argv[] = { arg0, arg1buf.data() };

    // Prepare blocking stdin so the application will wait until we push picks.
    static thread_local BlockingInputBuf inputBuf;
    auto* oldin = std::cin.rdbuf(&inputBuf);

    // Tee std::cout so we can observe output in real time.
    auto* oldOutBuf = std::cout.rdbuf();
    TeeBuf tee(oldOutBuf);
    std::cout.rdbuf(&tee);

    Lottery l;

    // Run the application in a separate thread so we can wait for READY and then inject stdin.
    auto thread_start = steady_clock::now();
    std::thread t([&]{
        try {
            l.initialize(2, argv, mode);
            l.run();
        } catch (...) {
        }
    });

    // Time until READY
    steady_clock::time_point ready_time;
    bool saw_ready = tee.wait_for("READY", seconds(30), ready_time);
    ASSERT_TRUE(saw_ready) << "Timed out waiting for READY for index " << idx;

    // Calculate time to READY
    auto us_to_ready = duration_cast<microseconds>(ready_time - thread_start).count();

    // Inject picks into stdin now that READY appeared.
    std::string picks = "1 2 3 4 5\n";
    inputBuf.push(picks);

    // Wait for expected_line to appear in output
    steady_clock::time_point result_time;
    bool saw_result = tee.wait_for(expected_line, seconds(30), result_time);
    ASSERT_TRUE(saw_result) << "Timed out waiting for expected result for index " << idx;

    // Print timings to stdout (these will be captured by the test runner)
    auto us_result_since_start = duration_cast<microseconds>(result_time - thread_start).count();
    std::cout << "INTEGRATION TEST: " << idx << " \033[32mPASSED" << "\033[0m" << std::endl;
    std::cout << "PLAYERS: " << l.getNumberOfPlayers() << std::endl;
    std::cout << "MODE: " << (mode==LotteryMode::BruteForce ? "BruteForce" : "Cached") << std::endl;
    std::cout << "READY: " << us_to_ready << "us" << std::endl;
    std::cout << "RESULT: " << (us_result_since_start - us_to_ready) << "us" << std::endl;
    std::cout << "TOTAL: " << us_result_since_start << "us" << std::endl << std::endl;

    // Verify expected_line present
    std::string out = tee.str();
    EXPECT_NE(out.find(expected_line), std::string::npos) << "Output did not contain expected for index " << idx;

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
            run_integration_test_for_idx_and_mode(i, mode);
        }
    }
}
