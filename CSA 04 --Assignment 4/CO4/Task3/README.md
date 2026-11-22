# Task 3 – Modern File Systems (Register No. 192565013)

## 3.1 Comparison (values are widely documented design limits; confirm any you cite)
| FS | Max file | Max volume | Journaling | Snapshots | Checksums | Copy-on-write |
|---|---|---|---|---|---|---|
| FAT32 | 4 GiB − 1 B | 2 TiB (with 512 B sectors; Windows formats up to 32 GB) | No | No | No | No |
| NTFS | ~16 EiB theoretical (~256 TiB in practice) | ~256 TiB (64 KB clusters) | Yes (metadata) | Via VSS | No (metadata only) | No |
| ext4 | 16 TiB (4 KB blocks) | 1 EiB | Yes | No (LVM snapshots) | Metadata/journal only | No |
| XFS | 8 EiB | 8 EiB | Yes (metadata) | No | Metadata (v5) | Reflink only |
| Btrfs | 16 EiB | 16 EiB | No (uses CoW) | Yes | Data + metadata | Yes |
| ZFS | 16 EiB | 256 ZiB | No (CoW + intent log) | Yes | Data + metadata | Yes |
| APFS | 8 EiB | 8 EiB | No (CoW metadata) | Yes | Metadata | Yes |

## 3.2 Power failure, journaling and CoW
If power fails mid-write, a file system without protection can have updated a bitmap but not the inode (leaked blocks) or the reverse (two files claiming one block); `fsck` must scan everything. **Journaling** writes the intended change to a log first; after the crash the log is replayed or discarded, so recovery takes time proportional to the log, O(journal size), not the disk. **Copy-on-write** never overwrites live blocks: new blocks are written and then one root pointer is atomically switched, so the file system is always consistent.

| ext4 mode | What is journaled | Safety | Speed |
|---|---|---|---|
| journal | data + metadata | highest (data written twice) | slowest |
| ordered (default) | metadata; data written before the metadata commits | good: no stale data exposed | medium |
| writeback | metadata only, no ordering | metadata consistent, file contents may be stale/garbage after a crash | fastest |

## 3.3 Choices
- **Pen drive (Windows+Linux):** exFAT (or FAT32 if devices are old) – readable on both, no 4 GiB limit with exFAT.
- **Linux database server:** ext4 (ordered) or XFS – predictable latency, mature journaling, good for large files.
- **Laptop:** ext4 for simplicity (Linux) / NTFS (Windows) / APFS (Mac).
- **Backup server with snapshots:** ZFS or Btrfs – cheap snapshots, checksums detect silent corruption.

## 3.4 Inspection
Sandbox output is in `inspection_sandbox.txt`. Repeat on your WSL: `stat <file>`, `df -i /`, `ls -i`, `sudo tune2fs -l /dev/sdX` (or `sudo dumpe2fs -h /dev/sdX`). In WSL2 the root is a virtual ext4 disk, usually `/dev/sdc`.
(`tune2fs`/`dumpe2fs` were refused in my sandbox, so there is no output for them here.)

## Alternative: CoW file system for the database
Small random updates: CoW writes new blocks and updates parent pointers (write amplification, fragmentation), so latency is usually higher than ext4's in-place update, O(tree depth) block writes vs O(1) data write + journal record. Space: snapshots keep old blocks (grows with change rate); checksums add roughly 0.1–1% overhead. Recovery after power failure: near-instant (switch to last consistent root) versus journal replay – both fast; ext4 without journal needs fsck.
