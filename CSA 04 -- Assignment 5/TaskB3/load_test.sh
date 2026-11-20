#!/usr/bin/env bash
set -euo pipefail
URL=${1:-http://127.0.0.1:8000/health}
if command -v hey >/dev/null 2>&1; then
  hey -n 10000 -c 100 "$URL"
elif command -v ab >/dev/null 2>&1; then
  ab -n 10000 -c 100 "$URL"
else
  echo "Install hey or apache2-utils (ab), then rerun: $0 [URL]" >&2
  exit 1
fi
