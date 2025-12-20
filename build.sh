#!/usr/bin/env bash
set -euo pipefail

BUILD_DIR=build
mkdir -p "$BUILD_DIR"

# Configure (defaults to Release as set in CMakeLists)
cmake -S . -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE=Release

# Build everything (parallel)
cmake --build "$BUILD_DIR" --config Release -j $(nproc)

echo "Build finished (build directory: $BUILD_DIR)" 
