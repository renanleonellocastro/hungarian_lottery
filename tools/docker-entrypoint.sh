#!/usr/bin/env bash
# Container entrypoint. With arguments it behaves exactly like the lottery
# executable. Without arguments it generates a demo players file and starts it.
set -euo pipefail

DEMO_PLAYERS="${DEMO_PLAYERS:-1000000}"
DEMO_DIR=/tmp/demo

if [ $# -gt 0 ]; then
  exec lottery "$@"
fi

python3 /opt/hungarian_lottery/generate_players.py \
  --players "$DEMO_PLAYERS" --draws 5 --seed 42 --out-dir "$DEMO_DIR" --no-expected >&2

echo "Type a draw (five distinct numbers from 1 to 90) after READY, for example:" >&2
sed 's/^/  /' "$DEMO_DIR/draws.txt" >&2
echo "Output: winners with 2 3 4 5 matches. Ctrl+D to quit." >&2

exec lottery "$DEMO_DIR/players.txt"
