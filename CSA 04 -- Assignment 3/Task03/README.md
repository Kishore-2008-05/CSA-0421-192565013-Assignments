# Q3 – Segmentation
Compile:
```bash
gcc -std=c11 -Wall -Wextra segmentation.c -o segmentation
gcc -std=c11 -Wall -Wextra segmentation_fragmentation.c -o segmentation_fragmentation
```
Run both. The first validates segment/offset pairs using base and limit. The second first-fit simulation loads 20 seeded random segments into a 10,000-unit region, removes every third segment and calculates free space, largest hole and an external-fragmentation indicator. Output is in `ci_fragmentation_output.txt`. This is a simulation, not the Linux kernel's physical memory allocator.