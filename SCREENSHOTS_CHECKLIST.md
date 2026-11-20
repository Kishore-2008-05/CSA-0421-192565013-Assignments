# Student-run screenshot checklist

The repository contains CI outputs and written instructions. The following screenshots must be captured on the student's own WSL/VM/cluster to prove individual execution.

## CO3
- Task01: static vs dynamic executable sizes, `ldd`, and `dlopen` output showing `/proc/self/maps`.
- Task02: paging output at 1 KiB and 2 KiB page sizes; `getconf PAGESIZE`; `/usr/bin/time -v` page-fault output.
- Task03: eight segment/offset translations and a controlled segmentation-fault child result; `/proc/<pid>/maps`.
- Task04: sequential/random memory experiment and fork copy-on-write measurement under a safe memory limit.
- Task05: FIFO/LRU/Optimal output, CSV opened in Excel with the faults-vs-frames chart.
- Task06: `lsblk -f`, `swapon --show`, RAID capacity calculator output.
- Task07: `ulimit -l` and mlock latency results; repeat trials and state limits.

## CO4
- Task01: directory tree and permission/ACL demonstration.
- Task02: file-allocation output, inode capacity and bitmap calculations.
- Task03: `stat`, `df -i`, `ls -i`, `findmnt -T .`, and ext filesystem metadata where applicable.
- Task04: six scheduling algorithms, 10,000-request model output, and actual `/sys/block/<device>/queue/scheduler`.
- Task05: storage architecture diagram and replication/consistency explanation.
- Task06: two workload designs, simulation output, and safe `fio` output from a dedicated test file/device.

## CO5
- TaskB1: `ps`, `top`, `free`, `vmstat`, `iostat`, `df`, `ss`; cron; systemd/Nginx/UFW status.
- TaskB2: Docker build, `docker ps`, `docker stats`, `docker history`, `docker images`, `docker exec ... whoami`.
- TaskB3: native/VM/container benchmark table; Kubernetes 3 Running Pods, delete/recreate, 6 replicas, resource requests/limits and HPA.
- TaskB4: portal and LMS both running with resource limits; capacity estimate and final decision.

Save images in the matching task folder using descriptive names (for example `screenshots/docker-stats.png`). Include the terminal command and output in a text file as well; screenshots alone are not sufficient.
