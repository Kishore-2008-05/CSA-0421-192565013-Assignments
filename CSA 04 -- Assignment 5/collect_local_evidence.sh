#!/usr/bin/env bash
set -u
OUT=${1:-local-evidence}
mkdir -p "$OUT"
{
  date -Is
  echo "=== OS ==="; uname -a
  if command -v lsb_release >/dev/null 2>&1; then lsb_release -a; fi
  echo "=== GCC ==="; gcc --version | head -1
  echo "=== CPU ==="; nproc; lscpu | head -25
  echo "=== Memory ==="; free -h
  echo "=== Processes ==="; ps aux --sort=-%cpu | head
  echo "=== VM stats ==="; vmstat 1 5
  echo "=== Disk ==="; df -hT
  echo "=== Network sockets ==="; ss -tulpn
  echo "=== Docker ==="
  if command -v docker >/dev/null 2>&1; then docker --version; docker info 2>&1 | head -30; else echo "Docker not installed"; fi
  echo "=== kubectl ==="
  if command -v kubectl >/dev/null 2>&1; then kubectl version --client; kubectl get nodes 2>&1; else echo "kubectl not installed"; fi
} > "$OUT/linux-platform.txt" 2>&1
echo "Saved $OUT/linux-platform.txt"
