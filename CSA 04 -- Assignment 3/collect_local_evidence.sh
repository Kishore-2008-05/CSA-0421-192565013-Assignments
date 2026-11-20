#!/usr/bin/env bash
set -u
OUT=${1:-local-evidence}
mkdir -p "$OUT"
{
  date -Is
  echo "=== OS ==="; uname -a
  if command -v lsb_release >/dev/null 2>&1; then lsb_release -a; fi
  echo "=== GCC ==="; gcc --version | head -1
  echo "=== Page size ==="; getconf PAGESIZE
  echo "=== Memory ==="; free -h
  echo "=== VM stats ==="; vmstat 1 5
  echo "=== Block devices ==="; lsblk -f
  echo "=== Swap ==="; swapon --show
  echo "=== Meminfo ==="; grep -E 'MemTotal|MemAvailable|SwapTotal|SwapFree' /proc/meminfo
} > "$OUT/system.txt" 2>&1
echo "Saved $OUT/system.txt"
