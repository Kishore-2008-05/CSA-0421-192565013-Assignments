# Q3 – Segmentation
Compile: `gcc -std=c11 -Wall -Wextra segmentation.c -o segmentation`
Run: `./segmentation`
The program checks (segment, offset) pairs against each segment's base and limit and prints physical addresses or a protection fault. Compare eight test cases by hand. For a real process map, run `cat /proc/<pid>/maps` while a long-running process is alive. For a controlled SIGSEGV demonstration, use a child process and inspect its exit status; do not crash the shell.