# Q2 – Paging
Compile:
```bash
gcc -std=c11 -Wall -Wextra paging_translation.c -o paging_translation
gcc -std=c11 -Wall -Wextra paging_fragmentation.c -o paging_fragmentation
```
Run both. The first translates logical addresses using the sample page table, including an invalid address and a page fault. The second uses fixed seed 192565013 to calculate internal fragmentation for 100 random process sizes with 1 KiB, 4 KiB and 2 MiB pages. Results are in `ci_fragmentation_output.csv`. Change PAGE_SIZE to 2048 in the translation example for the required second run. Collect `getconf PAGESIZE` and `/usr/bin/time -v` from your own Linux machine.