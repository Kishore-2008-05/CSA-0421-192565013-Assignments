#!/usr/bin/env bash
set -u
OUT=${1:-local-evidence}
mkdir -p "$OUT"
{
  date -Is
  echo "=== OS ==="; uname -a
  echo "=== GCC ==="; gcc --version | head -1
  echo "=== Filesystem mount ==="; findmnt -T .
  echo "=== Capacity ==="; df -hT .
  echo "=== Inodes ==="; df -i .
  echo "=== Directory inode ==="; ls -id .
  echo "=== Block devices ==="; lsblk -f
  echo "=== Disk scheduler candidates ==="
  for f in /sys/block/*/queue/scheduler; do echo "$f"; cat "$f"; done
} > "$OUT/filesystem-disk.txt" 2>&1
echo "Saved $OUT/filesystem-disk.txt"
