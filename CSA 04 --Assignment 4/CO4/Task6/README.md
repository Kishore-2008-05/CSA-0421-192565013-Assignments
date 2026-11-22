# Task 6 – Video-Streaming Storage Design (Register No. 192565013)
Everything numeric below is either a calculation from stated assumptions or output of `video_sim.c` (a *model*, using assumed device speeds – not measurements of real hardware). Run `fio` on your own machine for real device numbers.

## Stage 1 – Workload and testable requirements
Operations: upload once (write-once, large sequential), play (large sequential reads), seek to any minute (random read), delete rarely.
Assumptions: 2,000 videos × 1 GB average = 2 TB library; 5% of the 200,000 students (10,000) streaming at peak at 3 Mbps.
Requirements: (R1) playback starts within 2 s; (R2) seeking to any minute needs ≤ 3 disk reads; (R3) 10,000 concurrent streams; (R4) survive loss of one disk/server; (R5) hot videos served from SSD.

## Stage 2 – Architecture
Origin on HDD (cheap capacity) with an NVMe SSD tier caching the hottest videos; popular videos also in RAM page cache. File system: XFS (extent-based, large files) on each server; allocation: extents (contiguous where possible, indexed by extent tree) – sequential speed of contiguous with growth flexibility. Three replicas across servers.

## Stage 3 – Critical decisions
- **Block/chunk size:** 4 MB I/O chunk (filesystem block 4 KB, large readahead): amortises ~6 ms seek+rotation over a 27 ms transfer.
- **Disk scheduler:** `mq-deadline` for HDDs (sorting + deadlines avoid starvation), `none` for NVMe.
- **Caching:** LRU cache of whole popular videos on SSD (Zipf popularity: few videos get most views).
- **Replication factor:** 3.

## Stage 4 – Calculations
- Capacity: 2 TB ×3 replicas = 6 TB (+30% headroom ≈ 8 TB usable).
- Throughput: 10,000 × 3 Mbps = 30 Gbps = 3.75 GB/s. Each stream needs a 4 MB chunk every 4 MB ÷ 0.375 MB/s ≈ 10.7 s → ≈ 937 chunk requests/s.
- One HDD (model): ≈ 26.7 req/s (FCFS) → 36 HDDs without cache.
- Disk reads to start at any minute: byte offset = minute × bitrate → extent tree lookup (1–2 metadata reads, usually cached) + 1 data read = **2–3 reads**.

## Stage 5 – Simulation (`sim_output.txt`)
| Design | SSD hit ratio | Avg latency (model) | HDDs for 937 req/s |
|---|---|---|---|
| A: HDD only, FCFS | – | 37.44 ms | 36 |
| B1: +SSD cache 5% of library | 50.8% | 17.57 ms | 16 |
| B2: +10% | 61.5% | 14.20 ms | 13 |
| B3: +20% | 72.5% | 10.76 ms | 9 |
(Note: the SSD load in B2/B3 exceeds one NVMe's modelled ~476 req/s, so use 2 NVMe drives.) Design B3 is the choice: about 4× fewer disks and about 3.5× lower average latency in the model. Chart of these values: build from the table.

## Alternative: object storage + CDN
Start-up time: CDN edge near the student gives low latency (tens of ms) in every city; own servers are fast only near the campus. Cost (assumptions: 4 h/month/student at 2 Mbps = 3.6 GB, so 720 TB/month; storage $0.023/GB-month; CDN $0.05/GB – verify current prices): storage ≈ $46/month, transfer ≈ $36,000/month (`cost_estimate.txt`). Own servers' costs (hardware, bandwidth, staff) are not included, so no total comparison is claimed. Effort: the provider runs hardware and scaling; the college only manages content and cost. Complexity: lookup O(1) edge cache hit; space O(library) with the provider handling replicas.
