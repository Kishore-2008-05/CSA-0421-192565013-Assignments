# Task 5 – Distributed and Cloud Storage (Register No. 192565013)

## 5.1 Comparison
| | NFS | HDFS / Ceph | Object storage (S3/GCS) |
|---|---|---|---|
| Consistency | Close-to-open (NFSv4); strong per server | HDFS strong for single writer; Ceph strong | S3 now strongly consistent for reads-after-writes; some stores eventual |
| Availability | Single server = single point of failure (unless clustered) | Replicas/erasure coding survive node loss | Very high, managed by provider |
| Performance | Good on LAN, one server limits scaling | Scales with nodes; best for large sequential reads (HDFS), mixed (Ceph) | High throughput, higher per-request latency |
| Cost | Cheap, one copy | Hardware ×3 for replication, admin effort | Pay per GB and per transfer; no hardware |
Choice for a university with files from any campus/home: Ceph or cloud object storage behind a file gateway; NFS is simplest for one campus.

## 5.2 Replication and consistency
Keeping 3 copies on different nodes/racks (HDFS default) means one disk, node or even rack can fail without data loss, and reads can go to the nearest copy. Cost: 3× storage and every write must reach all replicas. **Strong consistency**: every read sees the latest write (writes wait for replicas; slower, may refuse service during partitions). **Eventual consistency**: replicas converge later (fast and always available, but readers can briefly see old data). This is the CAP trade-off.

## 5.3 Device management
- **Device driver:** hides hardware details behind a standard interface; the kernel calls generic read/write, the driver talks to the controller.
- **Block-device interface:** storage is addressed as numbered fixed-size blocks; file systems sit above it.
- **Buffering:** temporary memory to match speed differences or hold data in transit.
- **Caching:** keeping recently used blocks in RAM (page cache) to avoid disk access.
- **Spooling:** queueing output for a device that cannot be shared (e.g. printer) so programs do not wait.

## Alternative: one large NFS server
Read/write times grow with users: the single server's disk, CPU and network are shared, so per-user throughput ≈ T/users, O(users) degradation; a distributed system spreads load over nodes. Storage: one copy (1×) vs three replicas (3×). Availability: if the server fails, all users lose access until repair; replicas continue serving. Complexity: lookup O(1) per request in both, space O(data) vs O(3·data).
