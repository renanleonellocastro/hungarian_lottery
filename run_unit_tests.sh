#!/usr/bin/env bash
# Build (if needed) and run the unit test suite.
set -euo pipefail

BUILD_DIR=build

./build.sh
ctest --test-dir "$BUILD_DIR" --label-regex unit --output-on-failure "$@"
