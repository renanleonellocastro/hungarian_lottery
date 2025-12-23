#!/usr/bin/env bash

set -e

echo "[INFO] Generating Doxygen documentation..."

if ! command -v doxygen &> /dev/null; then
    echo "[ERROR] Doxygen is not installed."
    exit 1
fi

if [ ! -f Doxyfile ]; then
    echo "[INFO] Generating default Doxyfile..."
    doxygen -g
fi

doxygen Doxyfile

echo "[INFO] Documentation generated at docs/html/index.html"
