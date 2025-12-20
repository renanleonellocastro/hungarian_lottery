#!/usr/bin/env bash
set -euo pipefail

BUILD_DIR=build
INTEGRATION_TEST_BIN="$BUILD_DIR/tests/tests_integration"

# Ensure library is built
if [ ! -f "$BUILD_DIR/src/libhungarian_lottery.a" ]; then
  echo "Library not found; building library target..."
  ./build.sh
fi

if [ -x "$INTEGRATION_TEST_BIN" ]; then
  echo "Running integration tests via $INTEGRATION_TEST_BIN"
  "$INTEGRATION_TEST_BIN" --gtest_color=yes --gtest_output=xml:tests_integration_results.xml
else
  echo "Integration test binary $INTEGRATION_TEST_BIN does not exist or is not executable." >&2
  exit 1
fi
