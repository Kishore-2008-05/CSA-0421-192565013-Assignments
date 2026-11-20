#!/usr/bin/env bash
set -euo pipefail
THRESHOLD=${1:-80}
TARGET=${2:-/}
PCT=$(df -P "$TARGET" | awk 'NR==2 {gsub(/%/,"",$5); print $5}')
printf 'Filesystem usage for %s: %s%% (threshold %s%%)\n' "$TARGET" "$PCT" "$THRESHOLD"
if (( PCT >= THRESHOLD )); then
  printf 'ALERT: disk usage is at or above threshold\n'
  exit 2
else
  printf 'OK: disk usage is below threshold\n'
fi
