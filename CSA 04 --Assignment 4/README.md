# CO4 – Optimising File Storage and Disk Access

**Student:** Kishore Kumar S  
**Register number:** 192565013  
**Compiler:** GCC (record your local `gcc --version`)  
**OS:** Ubuntu on WSL (record `lsb_release -a` and `uname -a`).

Compile each C program:
```bash
gcc -std=c11 -Wall -Wextra program.c -o program
./program
```
Task02 contains first-fit/indexed allocation and inode/bitmap calculations. Task04 contains FCFS, SSTF, SCAN, C-SCAN, LOOK and C-LOOK. Task06 contains a seeded indexed-vs-linked seek-cost simulation.

For filesystem inspection use `stat`, `df -i`, `ls -i`, `findmnt -T .`; use `tune2fs -l` only after identifying an ext filesystem device. For disk scheduler evidence use `lsblk` and `cat /sys/block/<device>/queue/scheduler`.

See `COMPLETE_WRITTEN_REPORT.md` for all six task answers, calculations, comparisons and experiment instructions. Hardware-specific measurements and screenshots from the student's own WSL remain to be added.