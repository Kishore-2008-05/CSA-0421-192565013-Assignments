# Q7 – Memory-management plan for edge device
Compile: `gcc -std=c11 -Wall -Wextra mlock_latency.c -o mlock_latency`
Run: `ulimit -l` then `./mlock_latency 8`.
The program compares first-touch and locked repeated-touch latency. `mlock` may fail if the process memlock limit is too small. Record the actual limit, repeated trial count, median and worst observed latency. A short experiment is not proof of hard real-time worst-case execution time.