# Q6 – Disk management
Compile: `gcc -std=c11 -Wall -Wextra raid_capacity.c -o raid_capacity`
Run: `./raid_capacity`
The calculator compares nominal usable capacity for six 4-TB disks across RAID0, RAID1 (three mirrored pairs), RAID5, RAID6 and RAID10. It is a capacity calculation, not a live RAID test. Use `lsblk -f`, `swapon --show` and `sudo fdisk -l` to inspect your own system. Never create or modify RAID arrays on a disk containing data for this assignment.