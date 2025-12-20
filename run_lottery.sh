#!/usr/bin/env bash
set -euo pipefail

BUILD_DIR=build
BIN="$BUILD_DIR/lottery"

# Ensure library is built
if [ ! -f "$BUILD_DIR/src/libhungarian_lottery.a" ]; then
  echo "Library not found; building library target..."
  ./build.sh
fi

# Fail if binary application does not exist
if [ ! -x "$BIN" ]; then
    echo "Binary file $BIN does not exist or is not executable." >&2
    exit 1
fi

# Execute with any forwarded args
exec "$BIN" "$@"
