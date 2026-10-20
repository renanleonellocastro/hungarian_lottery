#!/usr/bin/env bash
# Build an instrumented Debug tree, run every test and produce a gcovr report.
# Output: coverage/index.html (HTML) and coverage/summary.json (totals).
set -euo pipefail

BUILD_DIR=build-coverage
REPORT_DIR=coverage

if ! command -v gcovr > /dev/null; then
  echo "gcovr is required (pip or apt package 'gcovr')" >&2
  exit 1
fi

cmake -S . -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE=Debug -DENABLE_COVERAGE=ON -DBUILD_BENCHMARKS=OFF > /dev/null
cmake --build "$BUILD_DIR" -j "$(nproc)" > /dev/null
ctest --test-dir "$BUILD_DIR" --output-on-failure > /dev/null

mkdir -p "$REPORT_DIR"
gcovr --root . --object-directory "$BUILD_DIR" \
  --filter 'src/' --filter 'include/' \
  --html-details "$REPORT_DIR/index.html" \
  --json-summary-pretty --json-summary "$REPORT_DIR/summary.json" \
  --print-summary
