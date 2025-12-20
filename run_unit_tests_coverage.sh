#!/usr/bin/env bash
set -euo pipefail

BUILD_DIR=build

# Ensure library is built
if [ ! -f "$BUILD_DIR/src/libhungarian_lottery.a" ]; then
  echo "Library not found; building library target..."
  ./build.sh
fi

# Configure with coverage enabled. CMakeLists will add -O0 -g --coverage when
# ENABLE_COVERAGE=ON for supported compilers. On success we print only the overall
# coverage summary. On failure we print the captured logs.
CONFIG_LOG=$(mktemp)
BUILD_LOG=$(mktemp)
trap 'rm -f "$CONFIG_LOG" "$BUILD_LOG"' EXIT

cmake -S . -B "$BUILD_DIR" -DENABLE_COVERAGE=ON -DCMAKE_BUILD_TYPE=Debug >"$CONFIG_LOG" 2>&1 || {
  cat "$CONFIG_LOG" >&2
  echo "CMake configuration failed" >&2
  exit 1
}

# Build the coverage target (tests/CMakeLists defines a `coverage` custom target)
cmake --build "$BUILD_DIR" --target coverage -- -j"$(nproc)" >"$BUILD_LOG" 2>&1 || {
  cat "$BUILD_LOG" >&2
  echo "Build or coverage collection failed" >&2
  exit 1
}

# Prefer the cleaned info file produced by the coverage target.
COV_INFO="$BUILD_DIR/coverage.info.cleaned"
RAW_INFO="$BUILD_DIR/coverage.info"

if [ -f "$COV_INFO" ]; then
  LCOV_OUT=$(lcov --summary "$COV_INFO" 2>&1) || { echo "$LCOV_OUT" >&2; exit 1; }
elif [ -f "$RAW_INFO" ]; then
  LCOV_OUT=$(lcov --summary "$RAW_INFO" 2>&1) || { echo "$LCOV_OUT" >&2; exit 1; }
else
  echo "Coverage outputs not found in $BUILD_DIR. Check that lcov/genhtml are installed." >&2
  exit 1
fi

# Extract the small summary block (the header and next two lines).
echo "$LCOV_OUT" | awk '/Summary coverage rate:/{print; if(getline) print; if(getline) print}'
