# CO3 – Memory Management (Register No. 192565013)

**Student:** Kishore Kumar S  
**Compiler:** GCC (record your own `gcc --version`)  
**OS:** Ubuntu on WSL (record `lsb_release -a`, `uname -a` and `getconf PAGESIZE`).

## Programs
- Task01: static/object linking, shared-library linking and runtime `dlopen/dlsym`.
- Task02: paging address translation and page fault/invalid address cases.
- Task03: segment-table base/limit translation and protection faults.
- Task04: demand paging/page-touch and fork copy-on-write demonstration.
- Task05: FIFO, LRU and Optimal page replacement, including Belady's anomaly.
- Task06: RAID capacity calculator for six 4-TB disks.
- Task07: memory locking (`mlock`) latency demonstration.

Compile ordinary C programs:
```bash
gcc -std=c11 -Wall -Wextra source.c -o program
./program
```
For Task01, follow `Task01/README.md` for shared-library and `-ldl` build commands.

## Written answers
See `COMPLETE_WRITTEN_REPORT.md` for all seven questions, evaluation, alternative concepts, calculations and experiment steps.

## Evidence still to collect locally
Record timings, RSS/PSS, page faults, real page size, disk/RAID and WSL system details on your own machine. Run each program and save terminal screenshots in the matching Task folder. Values depending on local hardware are not fabricated in this repository.
