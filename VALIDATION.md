# Automated validation record

**Workflow:** Compile assignment programs  
**Latest fully passing run:** [44757600619 – successful](https://github.com/Kishore-2008-05/CSA-0421-192565013-Assignments/actions/runs/44757138964)  
**Commit tested:** `ddca50f1f209b9d03a05ec18445f629d937c7031`

## Checks that passed
- GCC compilation of CO3 C programs, including static/object linking, shared-library linking, `dlopen/dlsym`, paging, segmentation, demand paging, page replacement and RAID capacity.
- GCC compilation of CO4 C programs, including allocation, inode/bitmap calculation, all six disk scheduling algorithms, 10,000-request queue simulation and storage workload simulation.
- Execution of the portable C programs and CO5 Bash log-analysis script.
- The runtime-linking demo printed the executable and shared-library regions from `/proc/self/maps`.
- The demand-paging demo touched 8 MiB and reported 2,047 minor faults and 0 major faults on that runner. The 100,000-reference three-policy page-replacement experiment took 12.111 ms on this runner (hardware dependent).
- The CO5 sample log summary matched the expected status counts, busiest client and total bytes.
- The paging-fragmentation experiment reported 53,150 B, 212,894 B and 107,413,406 B total internal waste for 1 KiB, 4 KiB and 2 MiB pages (fixed-seed 100-process sample).
- The segmentation simulation loaded 20 random segments, then reported 5,534 free units, a largest hole of 3,323 units and fragmentation indicator 0.3995.

## Selected verified outputs
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
medium load: FCFS average wait 2.923 ms; SSTF 0.882 ms
heavy load: FCFS average wait 8119.780 ms; SSTF 1513.895 ms
```
These waiting times use the synthetic model stated in `CSA 04 --Assignment 4/Task04/README.md`; they are not physical-disk measurements.
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

These are automated Ubuntu runner results, not screenshots or measurements from the student's WSL laptop. Docker/VM/Kubernetes deployment, real disk/filesystem inspection and hardware-specific timing/throughput experiments still need local execution.
