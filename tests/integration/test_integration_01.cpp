#include <gtest/gtest.h>
#include "integration_common.h"

TEST(Integration01, FullRun) {
    for (int i = 1; i < 21; ++i)
     run_integration_test(i);
}
