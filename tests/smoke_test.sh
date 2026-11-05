#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
TMP="$(mktemp -d)"
trap 'rm -rf "$TMP"' EXIT

g++ -std=c++17 -Wall -Wextra -Wpedantic -Werror \
  "$ROOT/src/main.cpp" -o "$TMP/sgpa-calculator"
"$TMP/sgpa-calculator" "$ROOT/data/academic_records.example.txt" "$TMP/report.txt"
grep -q 'SGPA: 3.625 / 4.000' "$TMP/report.txt"
grep -q 'SGPA: 4.000 / 4.000' "$TMP/report.txt"
echo 'PASS: SGPA calculator smoke test'
