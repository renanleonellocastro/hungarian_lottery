#!/usr/bin/env bash
# Remove every generated directory: build trees, test and coverage reports, generated docs.
set -euo pipefail

GENERATED_DIRS=(build build-coverage Testing coverage docs/html)

for dir in "${GENERATED_DIRS[@]}"; do
  if [ -d "$dir" ]; then
    echo "Removing $dir/"
    rm -rf "$dir"
  fi
done

echo "Clean complete."
