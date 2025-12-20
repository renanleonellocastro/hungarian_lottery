#!/usr/bin/env bash
set -euo pipefail

BUILD_DIR=build
TESTING_DIR=Testing

if [ -d "$BUILD_DIR" ]; then
  echo "Removing $BUILD_DIR/"
  rm -rf "$BUILD_DIR"
else
  echo "$BUILD_DIR/ does not exist"
fi

if [ -d "$TESTING_DIR" ]; then
  echo "Removing $TESTING_DIR/"
  rm -rf "$TESTING_DIR"
else
  echo "$TESTING_DIR/ does not exist"
fi

echo "Clean complete."
