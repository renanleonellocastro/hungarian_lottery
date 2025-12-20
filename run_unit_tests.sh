#!/usr/bin/env bash
set -euo pipefail

BUILD_DIR=build
UNIT_TEST_BIN="$BUILD_DIR/tests/tests_unit"

# Ensure library is built
if [ ! -f "$BUILD_DIR/src/libhungarian_lottery.a" ]; then
  echo "Library not found; building library target..."
  ./build.sh
fi

if [ -x "$UNIT_TEST_BIN" ]; then
  echo "Running unit tests via $UNIT_TEST_BIN"
  "$UNIT_TEST_BIN" --gtest_color=yes --gtest_output=xml:tests_unit_results.xml
else
  echo "Unit test binary $UNIT_TEST_BIN does not exist or is not executable." >&2
  exit 1
fi
