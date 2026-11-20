# Task 4 – Disk scheduling
Compile:
```bash
gcc -std=c11 -Wall -Wextra disk_scheduling.c -o disk_scheduling
gcc -std=c11 -Wall -Wextra disk_scheduling_all.c -o disk_scheduling_all
```
Run both programs. The all-algorithms version reports FCFS, SSTF, SCAN, LOOK, C-SCAN and C-LOOK for the specified queue. C-SCAN total includes the circular end-to-start movement correction. For the 10,000-request experiment, record a fixed random seed, request arrival model, average/max waiting time and load definition; the supplied static queue program alone is not a substitute for that experiment.