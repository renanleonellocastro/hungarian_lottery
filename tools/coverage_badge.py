#!/usr/bin/env python3
"""Convert a gcovr JSON summary into a shields.io endpoint badge.

Usage: coverage_badge.py <gcovr summary.json> <output badge.json>
The badge is served by GitHub Pages and rendered through
https://img.shields.io/endpoint?url=<pages url>/badges/coverage.json
"""

import json
import sys

COLOR_THRESHOLDS = [(90, "brightgreen"), (80, "green"), (70, "yellowgreen"), (60, "yellow"), (0, "red")]


def badge_color(percent):
    return next(color for threshold, color in COLOR_THRESHOLDS if percent >= threshold)


def main(summary_path, badge_path):
    with open(summary_path, encoding="utf-8") as summary_file:
        percent = float(json.load(summary_file)["line_percent"])

    badge = {"schemaVersion": 1, "label": "coverage", "message": f"{percent:.1f}%", "color": badge_color(percent)}

    with open(badge_path, "w", encoding="utf-8") as badge_file:
        json.dump(badge, badge_file)

    print(f"Line coverage {percent:.1f}% written to {badge_path}")
    return 0


if __name__ == "__main__":
    if len(sys.argv) != 3:
        sys.exit(__doc__)
    sys.exit(main(sys.argv[1], sys.argv[2]))
