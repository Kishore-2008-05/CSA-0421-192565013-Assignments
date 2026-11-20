# Automated validation record

**Workflow:** Compile assignment programs  
**Commit tested:** `dc21bef984246fbe9e6b2abfeec3e3697f30c9a5`  
**GitHub Actions run:** [44756513010 – successful](https://github.com/Kishore-2008-05/CSA-0421-192565013-Assignments/actions/runs/44756513010)

## Checks that passed
- GCC compilation of CO3 C programs: success.
- GCC compilation of CO4 C programs: success.
- Execution of paging translation, page replacement, segmentation, RAID capacity, file allocation, inode capacity, disk scheduling, storage workload and CO5 log summary: success.
- CO5 sample log summary matched expected status counts, busiest client and total bytes.

## Selected verified outputs from the runner
```text
3 frames: FIFO 15, LRU 12, Optimal 9
4 frames: FIFO 10, LRU 8, Optimal 8
Belady string, FIFO: 3 frames 9 faults, 4 frames 10 faults
```
```text
FCFS: 640 cylinders
SSTF: 236 cylinders
SCAN: 236 cylinders
LOOK: 208 cylinders
C-SCAN: 382 cylinders including circular wrap
C-LOOK: 322 cylinders including circular jump
```
```text
Requests per status code:
200 4
302 1
403 1
404 1
Busiest client:
10.0.0.5 4
Total bytes sent: 43200
```

These are automated Ubuntu runner results, not screenshots or measurements from the student's WSL laptop. Docker/VM/Kubernetes deployment and hardware-specific timing/throughput experiments remain to be executed locally.
