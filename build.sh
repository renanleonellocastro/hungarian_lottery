#!/usr/bin/env bash
# Configure and build the Release tree (application, tests and benchmark tool).
set -euo pipefail

BUILD_DIR=build

cmake -S . -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE=Release "$@"
cmake --build "$BUILD_DIR" --config Release -j "$(nproc)"

echo "Build finished (build directory: $BUILD_DIR)"
