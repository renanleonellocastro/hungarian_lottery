#!/usr/bin/env bash
# Black box test of the lottery executable over a generated dataset.
# Usage: cli_test.sh <lottery binary> <dataset directory>
set -euo pipefail

LOTTERY_BIN="$1"
DATASET_DIR="$2"

expected="$(printf 'READY\n'; cat "$DATASET_DIR/expected.txt")"
actual="$("$LOTTERY_BIN" "$DATASET_DIR/players.txt" < "$DATASET_DIR/draws.txt")"

if [ "$actual" != "$expected" ]; then
  echo "Output mismatch for $DATASET_DIR" >&2
  diff <(echo "$expected") <(echo "$actual") >&2 || true
  exit 1
fi

if echo "1 2 3 4 91" | "$LOTTERY_BIN" "$DATASET_DIR/players.txt" > /dev/null 2>&1; then
  echo "Expected a non zero exit status for an invalid draw" >&2
  exit 1
fi

if "$LOTTERY_BIN" > /dev/null 2>&1; then
  echo "Expected a non zero exit status when the players file is missing" >&2
  exit 1
fi

echo "CLI test passed for $DATASET_DIR"
