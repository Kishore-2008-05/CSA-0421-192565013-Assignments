# CO3 Q2 – Paging address translation

Compile and run:

```bash
gcc -std=c11 -Wall -Wextra paging_translation.c -o paging_translation
./paging_translation
```

Expected output with PAGE_SIZE=1024:

```text
0 -> page 0 offset 0 : physical 5120
1500 -> page 1 offset 476 : physical 2524
3100 -> page 3 offset 28 : physical 7196
6200 -> page 6 offset 56 : physical 3128
9000 -> page 8 offset 808 : invalid address
2049 -> page 2 offset 1 : page fault
```

For the requested 2048-byte experiment, change PAGE_SIZE to 2048 and rerun. Note that the given page table still has eight entries; addresses whose page number is 8 or more are outside this sample table.