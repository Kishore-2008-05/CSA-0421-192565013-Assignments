#!/usr/bin/env bash
# Print a warning for every mounted file system at or above THRESHOLD percent (default 80).
set -euo pipefail
THRESHOLD=${1:-80}
df -P -x tmpfs -x devtmpfs | awk -v t="$THRESHOLD" 'NR>1 { gsub("%","",$5); if ($5+0 >= t) printf "ALERT: %s is %s%% full (mounted on %s)\n", $1, $5, $6 }'
echo "disk check finished (threshold ${THRESHOLD}%)"
