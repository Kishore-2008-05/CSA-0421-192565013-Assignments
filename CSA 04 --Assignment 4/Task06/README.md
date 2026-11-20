# Task 6 – Video-streaming storage design
Compile: `gcc -std=c11 -Wall -Wextra storage_workload.c -o storage_workload`
Run: `./storage_workload`
The seeded simulation compares data-block reads for indexed/extent access and linked allocation for 100,000 random time seeks. The written design report includes architecture, capacity/throughput equations and a proposed HDD/SSD/cache/CDN layout. Run fio only against a dedicated test file or test volume; never target a production device.