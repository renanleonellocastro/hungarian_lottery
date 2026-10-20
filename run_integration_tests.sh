#!/usr/bin/env bash
# Build (if needed) and run the integration tests. The build generates the
# deterministic datasets under build/tests/data with tools/generate_players.py.
set -euo pipefail

BUILD_DIR=build

./build.sh
ctest --test-dir "$BUILD_DIR" --label-regex integration --output-on-failure "$@"
