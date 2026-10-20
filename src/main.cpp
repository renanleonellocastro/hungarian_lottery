#include <glog/logging.h>
#include <glog/log_severity.h>

#include "lottery.h"

#include <array>
#include <iostream>
#include <exception>


using namespace hungarian;

int main(int argc, char** argv) {
    google::InitGoogleLogging(argv[0]);
    FLAGS_logtostderr = 1;
    FLAGS_minloglevel = google::GLOG_ERROR;

    try {
        Lottery lottery;
        LotteryMode mode = LotteryMode::Cached;

        lottery.initialize(argc, argv, mode);

        while (lottery.run()) {
        }

    } catch (const std::exception& ex) {
        LOG(ERROR) << "Unhandled exception: " << ex.what();
        google::ShutdownGoogleLogging();
        return 1;
    }

    google::ShutdownGoogleLogging();
    return 0;
}
