#!/usr/bin/env bash
# ufw rules: allow only SSH, HTTP, HTTPS. Run with sudo on the real server.
set -euo pipefail
ufw default deny incoming
ufw default allow outgoing
ufw allow 22/tcp
ufw allow 80/tcp
ufw allow 443/tcp
ufw --force enable
ufw status verbose
