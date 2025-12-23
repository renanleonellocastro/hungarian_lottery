#!/usr/bin/env bash

set -euo pipefail

echo "[INFO] Generating Doxygen documentation..."

for cmd in doxygen dot; do
    if ! command -v "$cmd" &> /dev/null; then
        echo "[ERROR] Required tool not found: $cmd"
        exit 1
    fi
done

if [ ! -f Doxyfile ]; then
    echo "[ERROR] Doxyfile not found in project root"
    exit 1
fi

rm -rf docs
mkdir -p docs

doxygen Doxyfile

echo "[INFO] Documentation generated successfully"
echo "[INFO] Open: docs/html/index.html"
