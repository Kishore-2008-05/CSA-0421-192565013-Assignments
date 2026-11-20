# Q5 – Page replacement
Compile: `gcc -std=c11 -Wall -Wextra page_replacement.c -o page_replacement`
Run: `./page_replacement`
The implementation reports FIFO, LRU and Optimal page faults for 3/4 frames and demonstrates Belady's anomaly. For extended investigation, add locality-heavy and random reference generators and record fault counts for 3–10 frames plus runtime for 100,000 references. The written evaluation is in the parent complete report.