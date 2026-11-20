# CO3 Assignment 3 – Complete written solution and engineering analysis
**Student:** Kishore Kumar S  
**Register number:** 192565013  
**Course:** CSA04 Operating Systems  
**Status note:** Written analysis and source implementations are provided. Measurements that depend on the student's own WSL hardware (RSS/PSS, timings, real disk, page-fault counts under load) must be collected locally and must not be represented by invented values.

## Q1 – Linking, loading and address binding (6 marks)

### 1. Address binding and roles
- **Compile-time binding:** when the final memory location is known at compile time, the compiler/linker can emit absolute addresses. Moving the program requires recompilation.
- **Load-time binding:** relocatable code is adjusted by the loader when the executable is placed in memory.
- **Execution-time binding:** hardware address translation (MMU) maps virtual addresses while the process runs, allowing relocation and virtual memory.
- **Linker:** resolves external symbols, combines object files and libraries, and records dynamic dependencies.
- **Loader:** maps executable segments and shared libraries, applies relocations, builds the initial process image and transfers control to the entry point.

### 2. Static versus dynamic linking
| Criterion | Static | Dynamic/shared |
|---|---|---|
| Executable size | Usually larger; library code embedded | Usually smaller |
| Memory across processes | Duplicate private code possible | Read-only library pages can be shared |
| Start-up | No shared-library lookup for embedded code | Loader resolves/maps dependencies |
| Update | Relink/redeploy executable | Compatible shared library can be updated separately |
| Reliability | Fewer runtime library dependencies | Missing/incompatible .so can prevent start-up |
| Security maintenance | Rebuild every executable | Patch shared library and validate ABI compatibility |

Position-independent code (PIC) uses relocatable addressing so a shared object can be mapped at different virtual addresses. The Task01 source demonstrates `dlopen`, `dlsym`, error handling and a small shared object. Run `ldd`, compare `stat -c %s`, and inspect `/proc/<pid>/maps` while the process is alive. Use `/usr/bin/time -v` and `/usr/bin/time -f '%e %M'` for elapsed time and peak RSS; run multiple trials and report median values.

**Decision:** use dynamically linked, versioned libraries for the bank's common encryption component, with package-managed updates, compatibility tests and rollback. Use `dlopen()` for optional algorithms not needed on every start. If the library is absent, check `dlerror()`, disable the optional feature safely and log a clear error; never silently fall back to insecure encryption.

**Alternative – runtime loading:** `dlopen` postpones mapping and symbol resolution until encryption is requested. It reduces idle memory and can improve start-up for processes that rarely encrypt; the first encryption request incurs loading latency. Complexity of symbol lookup is effectively constant per symbol after loader work, while error-handling and ABI validation are additional engineering costs.

## Q2 – Paging (8 marks)

### 1. Fragmentation and translation
Contiguous allocation can leave free holes between allocated regions. A process needing one large contiguous region can fail even when total free memory is sufficient: this is external fragmentation. Paging divides virtual memory into fixed-size pages and physical memory into frames; any free frame can hold a page, removing external fragmentation. The final partially used page can waste space: internal fragmentation.

For page size 1024 bytes, logical address (A) gives page (p=lfloor A/1024floor), offset (d=A\bmod1024). If page-table entry (p) contains frame (f), physical address is (1024f+d). If the entry is absent, a page fault occurs. Task02 contains a runnable translation example and checks invalid page numbers.

### 2. Page-table size
For a 32-bit virtual address and 4 KiB pages: number of virtual pages (2^{32}/2^{12}=2^{20}). With 4-byte PTEs, a single-level table is (2^{20}\times4=4,194,304) bytes = **4 MiB per process**.
For a 48-bit address and 4 KiB pages: (2^{48-12}=2^{36}) entries. At 8 bytes/PTE, a flat table would be (2^{39}) bytes = **512 GiB**, which is impractical; multi-level tables allocate only populated portions. Actual x86-64 implementations use hierarchical tables and canonical virtual-address rules.

### 3. Effective access time with TLB
Let memory access time be (m), TLB lookup time (t), hit ratio (h). On a hit, time is (t+m); on a miss with a one-level page table, (t+2m). Thus (EAT=h(t+m)+(1-h)(t+2m)). State assumptions when using a different number of page-table levels. With (t=10ns,m=100ns), EAT is 130ns at 80%, 115ns at 95%, and 111ns at 99%.

### 4. Page sizes and internal fragmentation
For 100 processes with independently random final-page occupancy, expected waste is approximately half a page per process:
- 1 KiB pages: about 0.5 KiB × 100 = **50 KiB**.
- 4 KiB pages: about 2 KiB × 100 = **200 KiB**.
- 2 MiB pages: about 1 MiB × 100 = **100 MiB**.
This is an expectation, not a measured sample. Small pages reduce internal waste but increase page-table/TLB pressure; huge pages improve TLB reach and large sequential mappings but can waste memory and complicate allocation.

### 5. Page-table designs and protection
Multi-level tables split the virtual page number into indices and allocate lower-level tables only where mappings exist. Hashed page tables hash virtual page identifiers to entries, useful for large address spaces. Inverted page tables keep one entry per physical frame, reducing table size to physical-memory scale but requiring hash/search and process-ID tagging. PTE permissions (read/write/execute, user/supervisor) enforce protection. Shared libraries and shared-memory regions map the same physical frame into multiple processes, often read-only until copy-on-write.

### 6. Investigation
Run `getconf PAGESIZE`; run a test program using `/usr/bin/time -v ./program` and record minor/major faults. Test the first and last byte of a page and an address beyond the mapped region. Change Task02 PAGE_SIZE from 1024 to 2048 and rerun. Use a random-size generator with a fixed seed for repeatable fragmentation results.

**Conclusion:** 4 KiB is a practical general-purpose page size for a multi-user server; use huge pages selectively for large, long-lived database mappings after measurement.

**Alternative – inverted page table:** storage scales with physical frame count rather than each process's virtual address space. A hash lookup can be expected O(1), but collisions and shared mappings add work. A multi-level page-table walk is O(L) for L levels and benefits from TLB caching. Inverted tables save memory for huge sparse virtual spaces but complicate lookup and process isolation.

## Q3 – Segmentation (6 marks)

A segment is a logical region such as code, data or stack. A segment-table entry contains base and limit. For segment (s) and offset (d), first verify (0\le d<limit[s]); if valid, physical address is (base[s]+d), otherwise raise a protection fault. Task03 implements three segments and tests valid, boundary and invalid accesses.

Code can be read-only and shared; data is writable; stack can have a different growth policy. Segmentation matches the programmer's logical view and supports per-segment protection/sharing, but variable-sized allocation causes external fragmentation and may require compaction. Pure paging has fixed-size frames and no external fragmentation but has internal waste and page-table overhead. Segmented paging combines logical segments with page tables within each segment, reducing external fragmentation while preserving logical protection, at the cost of more translation structures.

**Experiment:** translate at least eight (segment, offset) pairs manually and compare with Task03. A real segmentation fault on modern Linux generally reflects an invalid virtual-memory access, not the old x86 segmentation model; demonstrate safely in a child process and inspect its exit status. Generate 20 random segment sizes with a fixed seed and simulate first-fit placement/removal to measure external holes.

**Conclusion:** modern general-purpose OSs rely mainly on paging because fixed-size frames simplify allocation, swapping and protection. Logical regions are represented through virtual-memory mappings rather than large variable-sized physical segments.

**Alternative – pure paging:** code/data/stack are page-aligned mappings with different permissions. Internal fragmentation replaces external fragmentation. Page-table size depends on mapped pages and hierarchy; translation is a TLB lookup plus page-table walk. Sharing code is straightforward by mapping identical read-only pages into processes, although logical segment boundaries are less explicit.

## Q4 – Virtual memory and demand paging (8 marks)

A page fault occurs when a process references a non-present page: CPU traps to kernel; kernel validates the address and permissions; it locates the page in a file or swap (or allocates a zero page); selects a frame (evicting a victim if necessary); reads/zeros the page; updates the PTE/TLB; and restarts the instruction. Invalid access produces a signal such as SIGSEGV.

### Effective access time
Memory time (=100ns), fault service (=8ms=8,000,000ns). Approximate (EAT=(1-p)100+p(8,000,000)):
- (p=10^{-3}): **8,099.9 ns ≈ 8.10 μs**.
- (p=10^{-5}): **179.999 ns ≈ 180 ns**.
- (p=10^{-6}): **107.9999 ns ≈ 108 ns**.
For at most 10% slowdown, EAT ≤ 110ns, so (p≤10/(8,000,000-100)≈1.25×10^{-6}).

A 24 GB process can run with 16 GB RAM if its active working set fits in available RAM and less-used pages can be backed by files/swap; it will perform poorly if it continually faults or if swap is absent/slow. Copy-on-write lets fork initially share physical pages read-only; a write causes a private copy. Thrashing occurs when working sets exceed available frames and the system spends most time paging. The working-set model tracks pages used in a recent window and guides frame allocation/admission control.

Equal allocation gives each process the same frame count; proportional allocation gives frames based on process size; priority/working-set allocation is often better for interactive and critical workloads. Detect thrashing using sustained major faults, swap-in/out, low useful CPU throughput and PSI; reduce multiprogramming or memory pressure.

Task04 touches one byte per page and reports minor/major fault deltas and a fork copy-on-write demonstration. Run with different memory sizes under a controlled VM/cgroup limit; do not deliberately exhaust the host. For 32 GB RAM, if the 24 GB workload's resident working set fits, faults reduce toward the no-fault baseline; the extra RAM cost must be compared with the measured time saved and workload frequency.

**Decision:** first improve locality, process only required image tiles and stream data; add RAM if the measured working set remains larger than physical memory. RAM avoids repeated disk faults, while locality improvements can reduce both RAM demand and I/O.

## Q5 – Page replacement (8 marks)

FIFO evicts the oldest loaded page; LRU evicts the least recently used page; Optimal evicts the page whose next use is farthest in the future. Optimal is a lower-bound benchmark but cannot be implemented online because future references are unknown. LRU has the stack property and does not exhibit Belady's anomaly, but exact LRU needs timestamps/list updates or hardware reference information. Clock approximates LRU using a reference bit and circular hand, reducing per-access overhead.

The supplied Task05 program reports:
- Given 20-reference string: 3 frames FIFO 15, LRU 12, Optimal 9; 4 frames FIFO 10, LRU 8, Optimal 8.
- Belady string: FIFO 3 frames 9 faults, 4 frames 10 faults.
FIFO can have more faults after adding a frame because its resident sets are not a stack. LRU avoids this due to inclusion of the smaller-frame resident set in the larger-frame set.

**Investigation:** generate deterministic locality-heavy and uniform-random streams; run 3–10 frames; plot faults; measure 100,000 references using a monotonic clock. Record seed, compiler flags and input size. For strong locality, LRU/Clock typically outperform FIFO; for random access, differences narrow. Recommend Clock or an LRU-family approximation for a general server because it captures recency with low kernel overhead; use measurements from the target workload.

**Alternative – Clock:** one reference bit per frame plus a hand; on fault, clear set bits while scanning and replace the first zero bit. A full scan can be O(F) in the worst case for F frames, but amortised work is low; exact LRU updates each access and needs ordering metadata. Clock uses O(F) bits/metadata and should be compared empirically with LRU on both reference streams.

## Q6 – Disk management (4 marks)

Low-level formatting creates sectors and headers; partitioning divides a disk into logical regions; logical formatting creates a file system. Firmware/boot manager locates a bootloader, which loads the kernel and initramfs; the kernel discovers devices, mounts root and starts init. Bad blocks are remapped by drive firmware or tracked by the file system; backups/RAID do not replace tested recovery. Swap can be a partition or file; size depends on peak anonymous memory and hibernation requirements. A separate disk can reduce contention but is not automatically faster; a swap file is easier to resize.

For six nominal 4 TB disks:
- RAID0: 24 TB, no fault tolerance.
- RAID1 as three mirrored pairs: 12 TB; can survive one failure per mirror pair.
- RAID5: 20 TB; survives one disk failure.
- RAID6: 16 TB; survives two disk failures.
- RAID10: 12 TB; survives at least one disk failure, depending on mirror placement.
RAID5/6 capacity assumes equal-size disks and decimal TB. RAID is not a backup. Use `lsblk -f`, `swapon --show`, `sudo fdisk -l`. Measure disk speed only on disks you own and avoid destructive write benchmarks on production devices.

**Decision:** for hospital records, choose RAID10 for write performance and fast rebuilds if budget/capacity permit, or RAID6 where capacity and two-disk fault tolerance dominate. Keep off-host, immutable backups and test restores. RAID6 over RAID5 adds one parity disk and can survive two failures, with extra parity calculation/write penalty.

## Q7 – Memory-management plan for 2 GB edge device (10 marks)

### Requirements and priorities
1. Real-time control task: deterministic deadline; must not incur page faults or swap; small bounded working set.
2. ML task: approximately 1.5 GB; can tolerate controlled latency and should be memory-capped.
3. Logger: low-priority buffered I/O; can tolerate delayed flushes but must not block control.
4. No swap device; reserve memory for kernel, drivers, DMA buffers and emergency margin.

### Design
Use Linux with a PREEMPT_RT-capable kernel if hard/firm real-time latency is required; pin the control thread to a CPU, use real-time scheduling only with a safe priority plan, pre-fault and lock its bounded memory using `mlockall(MCL_CURRENT|MCL_FUTURE)`, avoid dynamic allocation and blocking I/O in the control loop, and isolate CPU/memory resources. Apply cgroup limits to ML and logger. Use demand paging for noncritical ML data and streaming/mmap windows; keep logs bounded and asynchronously flushed. Do not lock the entire 1.5 GB ML workload if it starves the kernel.

Task07's `mlock_latency.c` compares first-touch and locked repeated-touch times. Run with a small bounded size and record `ulimit -l`; mlock can fail under the user's memlock limit. Worst-case latency requires repeated long-duration tests under CPU, I/O and memory stress; a single run is not a WCET proof.

### Mechanism comparison
| Mechanism | Real-time control | ML | Logger |
|---|---|---|---|
| Contiguous allocation | predictable but hard to grow | inflexible | unnecessary |
| Paging | risk of page fault | suitable | suitable |
| Segmentation | logical isolation, external fragmentation | not typical modern Linux allocation | not needed |
| Demand paging | avoid in critical loop | useful for large datasets | useful |
| Locked pages | recommended for bounded critical set | only selectively | generally not |

### Fourth 500 MB task
2 GB total minus 1.5 GB ML leaves only 0.5 GB for control, logger, kernel and all other services; a 500 MB additional task cannot be admitted safely without reducing ML's resident set, streaming its data, offloading work or increasing RAM. Define a hard memory budget and reject/queue workloads that exceed it.

**Alternative – RTOS without virtual memory:** removes demand-paging uncertainty and can provide more predictable control latency, but the ML task must fit into the remaining fixed physical memory; adding tasks requires static memory budgeting and careful integration. A hybrid architecture (RTOS control MCU plus Linux ML gateway) is a defensible option. Remaining limitation: Linux with mlock and tuning is not automatically hard real-time; prove deadlines on target hardware or move the control loop to a dedicated RTOS/MCU.

## Required local evidence checklist
- Record `gcc --version`, `uname -a`, `lsb_release -a`, `getconf PAGESIZE`.
- Run each C program using `gcc -std=c11 -Wall -Wextra file.c -o program` and save stdout/stderr.
- Capture screenshots of the actual terminal on your own WSL machine.
- Record timing methodology, sample count, CPU/RAM details and median/worst observed value.
- Do not submit the illustrative calculations above as measured data; label estimates separately from experiments.
