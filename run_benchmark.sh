#!/usr/bin/env bash
# Generate deterministic datasets and benchmark both strategies on them.
# Usage: ./run_benchmark.sh [player counts...]   (default: 1000000 10000000)
set -euo pipefail

BUILD_DIR=build
DATA_DIR=benchmarks/data
DRAWS=20
REPETITIONS=5
SIZES=("$@")

if [ ${#SIZES[@]} -eq 0 ]; then
  SIZES=(1000000 10000000)
fi

cmake -S . -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=OFF > /dev/null
cmake --build "$BUILD_DIR" --target lottery_benchmark -j "$(nproc)" > /dev/null

for size in "${SIZES[@]}"; do
  dataset="$DATA_DIR/$size"

  if [ ! -f "$dataset/draws.txt" ]; then
    python3 tools/generate_players.py --players "$size" --draws "$DRAWS" --seed 2024 --out-dir "$dataset" --no-expected
  fi

  echo
  echo "### $size players, $DRAWS draws x $REPETITIONS repetitions"
  "$BUILD_DIR/lottery_benchmark" "$dataset/players.txt" "$dataset/draws.txt" "$REPETITIONS"
done
