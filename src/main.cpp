#include <iostream>
#include <array>
#include <exception>

#include "lottery.h"
#include "logging_shim.h"

using namespace hungarian;

int main(int argc, char** argv) {
#ifdef HAVE_GLOG
    google::InitGoogleLogging(argv[0]);
    FLAGS_logtostderr = 1;
    FLAGS_minloglevel = google::GLOG_ERROR;
#endif

    try {
        Lottery lottery;
        LotteryMode mode = LotteryMode::Cached;
        lottery.run(argc, argv, mode);
    } catch (const std::exception& ex) {
        LOG(FATAL) << "Unhandled exception: " << ex.what();
        return 1;
    }

    return 0;
}
