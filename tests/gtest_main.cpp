#include <glog/logging.h>
#include <glog/log_severity.h>
#include <gtest/gtest.h>

int main(int argc, char** argv) {
    google::InitGoogleLogging(argv[0]);
    FLAGS_logtostderr = 1;
    FLAGS_minloglevel = google::GLOG_ERROR;

    ::testing::InitGoogleTest(&argc, argv);
    int rc = RUN_ALL_TESTS();

    google::ShutdownGoogleLogging();
    return rc;
}
