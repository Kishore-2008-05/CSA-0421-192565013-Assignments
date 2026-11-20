# Q4 – Virtual memory and demand paging
Compile: `gcc -std=c11 -Wall -Wextra demand_paging.c -o demand_paging`
Run: `./demand_paging 256` (argument is MiB to touch).
The program touches one byte per system page, reports minor/major fault deltas and forks a child that writes a page to demonstrate copy-on-write. Use `/usr/bin/time -v ./demand_paging 256` and controlled VM/cgroup memory limits for further tests. Do not intentionally exhaust the host's RAM.