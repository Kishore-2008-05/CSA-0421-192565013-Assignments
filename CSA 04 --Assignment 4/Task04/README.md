# Task 4 – Disk scheduling
Compile:
```bash
gcc -std=c11 -Wall -Wextra disk_scheduling.c -o disk_scheduling
gcc -std=c11 -Wall -Wextra disk_scheduling_all.c -o disk_scheduling_all
gcc -std=c11 -Wall -Wextra disk_queue_simulation.c -o disk_queue_simulation
```
Run all three. The static algorithm program reports FCFS, SSTF, SCAN, C-SCAN, LOOK and C-LOOK. The 10,000-request simulation compares FCFS and SSTF under seeded light/medium/heavy arrival rates. It uses an explicit synthetic service model (0.02 ms/cylinder plus 0.5 ms transfer), not physical-disk timings. See `disk_queue_simulation.ci_output.txt`; rerun to reproduce. For real device evidence, record the actual scheduler and run a safe benchmark on a dedicated test file/device.
