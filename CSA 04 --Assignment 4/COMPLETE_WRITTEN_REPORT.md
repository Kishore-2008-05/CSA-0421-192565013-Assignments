# CO4 Assignment 4 – Complete written solution and engineering analysis
**Student:** Kishore Kumar S | **Register:** 192565013 | **Course:** CSA04 Operating Systems

> Evidence integrity: formulas and design conclusions below are analytical. Device-specific measurements (disk scheduler, filesystem metadata, fio throughput, 10,000-request simulations) must be run and captured on the student's own Linux machine. Do not label estimates as measurements.

## Task 1 – File-system design review (6 marks)

### 1.1 Weaknesses of the proposal
1. A flat directory with 12,000 students creates name collisions and poor administration.
2. Searching and listing a very large directory can become costly; a student cannot predict another student's naming choice.
3. Contiguous allocation suffers external fragmentation and makes file growth difficult; relocation may be required.
4. No access control violates confidentiality and integrity; accidental or malicious reads/writes are possible.
5. Backup and restore become difficult because there is no clear ownership or retention boundary.
6. Group-project sharing is unsafe if permissions are absent or files are copied manually.
7. No quota or audit trail allows one user to exhaust shared storage and makes incidents hard to investigate.

### 1.2 Improved organisation
Use a hierarchy such as `/srv/student-data/<cohort>/<register-number>/{home,assignments,projects}`. Use stable register-number identifiers rather than user-selected names for directory identity; retain descriptive file names with validation and versioning. Store owner/group/mode, ACLs, timestamps, size, checksum and retention metadata. Set each home directory to owner-only (0700); shared course directories can be group-readable; project directories use a dedicated group or ACL. Create a shared project directory once and grant named group access rather than duplicating copies. Apply quotas, snapshots, off-host backups, audit logging and restore tests. Use ext4/XFS with indexed directories and journaling for general server storage.

### 1.3 Defence (under 100 words)
Each student receives a private directory owned by their account, with restrictive default permissions and quotas. Course staff receive explicitly granted read access where required. Group projects use a separate directory with a dedicated group or ACL, so only approved members can collaborate. Stable register-number paths prevent collisions, while audit logs and backups support accountability and recovery.

**Alternative – Drive/OneDrive:** cloud sync and indexing make remote access and search convenient, and replicas/version history can aid recovery. Storage may include local cache, version history and provider-side redundancy, so physical copies exceed logical file size. The college must evaluate data residency, account lifecycle, encryption, sharing defaults, retention, e-discovery and provider lock-in. Cloud does not remove the need for least-privilege access review.

## Task 2 – File allocation for 32-camera NVR (8 marks)

### 2.1 Operations
32 continuously growing recordings produce sequential writes and sequential playback; time-based search requires indexed direct access to a segment/offset. Deletion after 30 days is periodic and predictable. Metadata operations include create/close, extend, index time, seek and unlink. Design a time-based directory/manifest and preallocate large recording segments where practical.

### 2.2 Allocation comparison
| Method | Sequential | Direct/time seek | Growth | Fragmentation/reliability |
|---|---|---|---|---|
| Contiguous | Excellent | O(1) block arithmetic | Difficult unless extent reserved | External fragmentation; extent metadata can be compact |
| Linked | Good sequential | O(k) traversal | Easy | No external fragmentation; pointer corruption can lose remainder |
| FAT | Good sequential with cached FAT | O(k) chain traversal unless checkpoints | Easy | FAT can become large and is a critical metadata structure |
| Indexed/inode extents | Good | Index/extent lookup, typically a few reads | Good | Index overhead; extent trees support large runs |

For continuously growing video, use an extent-based file system (XFS/ext4) and segment recordings into fixed-duration files; maintain a time-to-file/offset index. This limits growth and deletion costs.

### 2.3 Cost calculation
4 GiB / 4 KiB = (2^{32}/2^{12}=2^{20}=1,048,576) blocks.
- Contiguous: calculate last block address directly; one data-block read after metadata (constant-time seek).
- Linked: reaching the last block requires following approximately 1,048,575 links (and then reading the target block); O(k) block accesses.
- Indexed: one index-block read for a single-level index if it covers the file; for multi-level/inode trees, at most a small number of index reads (plus target data read), depending on depth.

For 4 KiB blocks and 4-byte pointers, one index block holds 1024 pointers. Inode with 12 direct, one single, one double and one triple indirect pointer supports:
(12 + 1024 + 1024^2 + 1024^3) data blocks.
At 4096 bytes/block this is 49,152 + 4,194,304 + 4,294,967,296 + 4,398,046,511,104 bytes ≈ **4 TiB + 4 GiB + 4 MiB + 48 KiB** (ignoring filesystem limits and metadata).

### 2.4 Free-space management
For 4 TB decimal disk and 4 KiB blocks, block count = (4×10^{12}/4096≈976,562,500). A bit vector needs one bit per block: 122,070,313 bytes ≈ **116.4 MiB** (decimal bytes; about 116.4 MiB). If using 4 TiB, the bitmap is 128 MiB. A linked free list uses little in-memory metadata but requires traversal; grouping stores batches of free block addresses; counting stores start+length runs and works well for contiguous free extents. Bitmaps support efficient scanning and locality.

### Complete-code program
Task02 implements first-fit contiguous allocation and indexed allocation. The expected state after A/B/C/D allocation and B/D deletion is `AAAAAA........CCCCC.............`; the code then allocates E contiguously at the first run that fits and demonstrates the index-block/data-block representation. Compile with `gcc -std=c11 -Wall -Wextra file_allocation.c -o file_allocation`.

**Alternative – FAT:** a FAT held in memory makes each next-block lookup a RAM table access but reaching block k still follows k links: O(k). For a 4 TB disk with 4 KiB clusters, approximately 976.6 million entries are required. With 4-byte FAT entries, this is about 3.91 GB decimal (~3.64 GiB) of RAM. A damaged FAT can corrupt many file chains; keep redundant FAT copies and robust backups. Indexed/extent structures use a small number of tree/index reads for random access.

## Task 3 – Modern file systems (9 marks)

### 3.1 Comparison
| File system | Platforms | Journaling/consistency | Snapshots | Checksums / CoW | Typical role |
|---|---|---|---|---|---|
| FAT32 | Broad compatibility | No metadata journal | No native | No | Removable media; 4 GiB max file |
| NTFS | Windows; Linux support | Metadata journaling | VSS via Windows stack | Not general end-to-end data checksums; not CoW | Windows system/data volumes |
| ext4 | Linux | Journal; ordered default | No native general snapshots (use LVM) | No general data checksums; not CoW | General Linux server |
| XFS | Linux | Metadata journal | Reflink/CoW support; snapshots via LVM | Metadata checksums; CoW reflink | Large files and high-throughput Linux |
| Btrfs | Linux | CoW transaction model | Native subvolume snapshots | Checksums; CoW | Linux systems needing snapshots |
| ZFS | Cross-platform implementations | Copy-on-write transactional | Native snapshots/clones | End-to-end checksums; CoW | Storage/backup server |
| APFS | Apple platforms | CoW metadata | Native snapshots | Checksums for metadata; CoW | macOS/iOS storage |

Maximum file/volume sizes depend on implementation, block/cluster size and OS version; do not treat one universal limit as valid across all configurations. FAT32's 4 GiB minus one byte file-size limit is the notable interoperability constraint.

### 3.2 Crash consistency
A power failure during a write can leave metadata inconsistent (for example, an allocated block without a directory entry). Journaling records intended metadata updates so recovery can replay or roll back transactions. ext4 modes:
- `data=journal`: data and metadata are journaled; strongest consistency, more write overhead.
- `data=ordered`: data blocks are written before related metadata commits; default in many Linux deployments.
- `data=writeback`: metadata is journaled without ordering data first; faster in some workloads but stale data may appear after a crash.
CoW writes new blocks and atomically switches metadata roots/transactions, avoiding in-place overwrite of the only valid copy; it still needs correct hardware flush/barrier behavior and backups.

### 3.3 Device choices
- Pen drive shared between Windows/Linux: exFAT for large files and compatibility; FAT32 only when legacy devices require it.
- Linux database server: ext4 or XFS with tested database settings and power-loss protection; XFS for large files/parallel I/O, ext4 for broad operational familiarity.
- Laptop: NTFS for Windows, APFS for macOS, ext4 for Linux.
- Backup server needing snapshots/checksums: ZFS or Btrfs; ZFS is strong where operational expertise and RAM are available.

### 3.4 Inspect a real file system
Run `stat file`, `df -i`, `ls -i file`, `findmnt -T .`, and (only for ext filesystems) `sudo tune2fs -l /dev/<partition>` or `sudo dumpe2fs -h`. Record actual inode count, block size, free blocks and mount options. Do not run tune2fs on an unverified device.

**Alternative – Btrfs/ZFS:** CoW and checksums improve snapshot consistency and detect corruption, but small random overwrites may write new blocks and update metadata trees, increasing write amplification. Snapshots initially share blocks but consume extra space as data changes. Recovery uses transaction roots/logs and can be quick, but scrubs/resilvering can take time. Measure workload-specific write latency and snapshot growth before choosing.

## Task 4 – Disk scheduling (7 marks)

Given cylinders 0–199, initial head 53 and queue 98,183,37,122,14,124,65,67:
- FCFS order: 98,183,37,122,14,124,65,67; movement **640**.
- SSTF order: 65,67,37,14,98,122,124,183; movement **236**.
- SCAN moving toward 0 first: 37,14,0,65,67,98,122,124,183; movement **236** (includes travel to cylinder 0).
- C-SCAN moving upward: 65,67,98,122,124,183,199,0,14,37; movement **382** including wrap from 199 to 0.
- LOOK moving toward 0 first: 37,14,65,67,98,122,124,183; movement **208** (does not travel to cylinder 0).
- C-LOOK moving upward: 65,67,98,122,124,183,14,37; movement **322** including circular jump from 183 to 14.

The total for C-SCAN explicitly includes the end-to-start wrap. Some classroom conventions omit wrap distance from the reported service movement; state the convention clearly.

SSTF may starve a far-away request if nearer requests keep arriving. SCAN/C-SCAN provide bounded sweep-based service; C-SCAN gives more uniform waiting by serving in one direction and returning without servicing on the reverse sweep. LOOK/C-LOOK avoid unnecessary travel to physical disk ends. On SSDs, seek time is negligible; Linux commonly uses `none` or `mq-deadline` depending on device and workload. Check `cat /sys/block/<device>/queue/scheduler` after identifying the correct device with `lsblk`.

Task04 includes SSTF and all six static algorithms, plus a 10,000-request seeded FCFS/SSTF event simulation. The synthetic model uses mean inter-arrival gaps of 20 ms (light), 2 ms (medium), 0.2 ms (heavy), and service time 0.5 ms + 0.02 ms per cylinder moved. The reproducible output is in Task04/disk_queue_simulation.ci_output.txt. Under this model, light load has no queue wait; medium load FCFS/SSTF average waits are 2.923/0.882 ms; heavy load average waits are 8119.780/1513.895 ms and maximum waits 16258.200/4581.880 ms. These are simulated values under the stated assumptions, not physical disk measurements. A real device benchmark must be separately recorded.

**Alternative – LOOK/C-LOOK:** LOOK stops at the last pending request in the current direction, reducing end-of-disk travel. For this queue LOOK is 208 cylinders and C-LOOK 322 including circular jump. Finding the last request in an unsorted queue costs O(n); maintaining a sorted queue costs O(log n) insertion and O(1) endpoint selection, with O(n log n) initial sorting.

## Task 5 – Distributed and cloud storage (7 marks)

NFS exports a shared file namespace and is simple for campus LAN/home VPN use, but server/network availability and metadata throughput can bottleneck. HDFS is designed for large sequential datasets and replication, not low-latency POSIX random file updates. Ceph provides distributed block/file/object services with scale-out placement and self-healing, but needs operational expertise. Cloud object storage offers elastic durability and remote access through APIs, but has request/egress cost and object semantics rather than ordinary POSIX filesystem semantics.

Replication (for example, three HDFS copies) allows reads from another replica after a node/disk failure and increases storage use roughly threefold before compression/erasure coding. Strong consistency gives clients a current ordered view but can add coordination/latency. Eventual consistency allows temporary stale reads while replicas converge; suitability depends on metadata and application semantics. Storage device path: application issues I/O through system calls/VFS; filesystem maps file offsets to blocks; block layer queues requests; device driver submits commands; DMA/interrupts complete I/O. Buffering batches transfers, caching avoids repeated reads, spooling queues work for a device.

**Alternative – one NFS server:** initial setup is simpler and storage is one logical copy (plus backups), but throughput/metadata contention grows with clients and one server/network failure can make all files unavailable. A replicated distributed system uses more capacity and operational effort but can continue after node failure and scale out.

## Task 6 – Video-streaming storage design (13 marks)

### Stage 1 – Workload and targets
200,000 students; each video 300 MB–2 GB, uploaded once and read thousands of times, mostly in week one; students seek to arbitrary minute. Workload is write-once/read-many with random-range reads and bursty popularity. Define measurable targets: e.g. p95 time-to-first-byte under 2 s at target concurrency, seek response under 1 s, availability 99.9% during teaching windows, and no acknowledged upload loss. These are proposed service objectives to validate, not measured claims.

### Stage 2 – Architecture
Store durable originals in cloud object storage or a replicated on-prem object store; serve through CDN. Keep metadata/catalog and manifests in a database. Cache popular content on SSD/NVMe near origin; use HDD/object storage for cold originals. Use XFS/ext4 for local cache and extent-based files; object chunks/ranges support seeking. Avoid a single flat directory: partition by course/video ID and lifecycle date.

### Stage 3 – Decisions
Use 4 KiB filesystem blocks for general metadata and storage; object/chunk size e.g. 8–16 MiB as a tunable streaming segment, not as an OS block size. Use `none`/appropriate NVMe scheduler after measurement. Cache hot first-week videos using LFU/LRU hybrid with TTL and pre-warming. Replicate metadata and maintain at least two independent copies of origin data; object-store durability/replication settings must be verified against provider contract.

### Stage 4 – Capacity and throughput calculations
Let total stored logical video bytes be (V=sum_i size_i). For (N) concurrent viewers at bitrate (b), required delivery throughput is (N×b). Example planning scenario: 10,000 concurrent streams × 5 Mbit/s = 50 Gbit/s aggregate delivery; CDN edge capacity should absorb most of this, with origin shield sized for cache misses. At 200,000 students, if each video is 1 GB and there are 500 videos, raw content is 500 GB; two copies require about 1 TB before metadata/versioning. For arbitrary seek, a range request can retrieve the chunk containing the timestamp; with 8 MiB segments, maximum waste before requested point is less than one segment, and the index maps timestamp to segment/byte offset. A contiguous local file needs a direct offset calculation; linked allocation may require traversing many pointers, while indexed/extents need only a small index lookup.

### Stage 5 – Simulation plan
Use Task02 allocation simulation and Task04 scheduling simulator, plus `fio` only on a dedicated test file/device. Compare design A: HDD-only origin with no cache; design B: SSD hot cache + HDD/object cold tier. Record random seed, read size, queue depth, cache hit ratio, throughput, p95 latency and CPU. Never run destructive `fio` against a production block device. Actual measured results remain to be collected on student hardware.

**Alternative – cloud object storage + CDN:** students in different cities retrieve from nearby edge caches, reducing origin distance and improving start time on cache hits. Monthly storage cost = GB-month × provider rate; transfer cost = GB egress × regional rate; request cost = GET/PUT count × request rate. Exact INR estimates require the selected provider, region, stored GB, average watch hours, bitrate and egress. Operational effort is lower than operating disks/replication, but vendor dependence, data residency, egress cost and access control need review. For a small team, cloud object storage + CDN is the preferred starting design, with on-prem cache only if measured economics justify it.
