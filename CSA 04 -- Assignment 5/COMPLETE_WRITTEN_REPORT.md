# CO5 Assignment 5 – Complete written solution and engineering analysis
**Student:** Kishore Kumar S | **Register:** 192565013 | **Course:** CSA04 Operating Systems

> Experimental integrity: the B1 script is executable and its supplied sample-log output can be verified. Docker, VirtualBox/KVM and Kubernetes require services/tools not installed in this test environment. Their commands and configurations are supplied, but their execution, resource measurements, pod deletion/recreation and load-test numbers must be captured on the student's Ubuntu/cluster before submission.

# PART A – DESIGN (25 marks)

## A1 – Evaluate Linux system components (7 marks)

### A1.1 Architecture
Linux is a monolithic but modular kernel. User applications call system libraries (for example glibc), which wrap system calls. The shell interprets commands and scripts; utilities provide administration and text processing. The kernel manages:
- Processes/threads: creation, scheduling, context switching, signals and isolation.
- Memory: virtual address spaces, page tables, demand paging, page cache, reclaim and swap.
- Files: VFS abstracts ext4/XFS and other file systems; permissions and file descriptors control access.
- Devices: drivers expose block, character and network interfaces; interrupts and DMA move data.
- Networking: sockets, routing, TCP/IP, firewall hooks and network device queues.

### A1.2 CFS, cache, swap and observability
The Completely Fair Scheduler (CFS) historically tracks virtual runtime and chooses runnable tasks to approximate fair CPU sharing; modern Linux versions are transitioning scheduler internals (including EEVDF), so describe the kernel version actually installed rather than assuming all releases use identical internals. The page cache keeps file data in RAM to reduce disk reads. Swapping moves anonymous pages to backing storage under pressure; heavy swap activity causes latency. `/proc` exposes process/kernel runtime information (for example /proc/meminfo, /proc/<pid>/status, /proc/<pid>/maps); `/sys` exposes structured device and kernel attributes, often writable with privileges.

### A1.3 File hierarchy, VFS and permissions
`/` is the root; `/etc` configuration, `/home` user homes, `/var` logs/state, `/usr` installed programs/libraries, `/dev` device nodes, `/proc` and `/sys` virtual filesystems, `/tmp` temporary files. VFS gives applications a common interface across file-system implementations. Permission bits assign read/write/execute to owner/group/others; setuid executes a program with file-owner effective identity and must be tightly controlled. `sudo` grants audited, policy-limited administrative commands.

**Alternative – Windows Server:** Windows uses NT kernel, NTFS, PowerShell and service control manager. It may require a larger base image/disk footprint than a minimal Linux server, but exact RAM/disk figures depend on edition, roles and installed updates and must be measured on matching VMs. PowerShell/Group Policy/DSC can automate tasks; administration time depends on existing skills. Windows Server licensing and CAL costs are edition/core/user dependent and must be quoted from the current campus agreement. Linux is preferred for this Python portal where the team can support it: open tooling, automation, container ecosystem and no per-core OS licence charge; Windows may be preferable if application dependencies or institutional support require it.

## A2 – System administration design (5 marks)

### A2.1 Users, services and logs
Create separate non-login service account `portal`; group `portal-readers` for approved operators; `portal-admins` for limited administrators. Store secrets outside source control (root-owned environment file or secret manager). Run Gunicorn under systemd with `User=portal`, `Group=portal`, `WorkingDirectory=/opt/result-portal`, `Restart=on-failure`, resource limits and a restricted filesystem. Use journald with retention/forwarding; alert on repeated 5xx, authentication failures, service restarts and disk thresholds.

### A2.2 Networking
Use private RFC1918 addressing for app/database nodes, static DHCP reservations or cloud VPC addresses, DNS name such as results.university.example, Nginx reverse proxy for TLS termination and upstream load balancing. Firewall default-deny inbound; allow TCP 80/443 to proxy and SSH 22 only from VPN/admin subnet; app port only from proxy/security group; database port only from app tier. Disable password SSH login, use key authentication, restrict root login and rotate keys. Use TLS certificates and automated renewal.

### A2.3 Automation
Shell/cron or systemd timers: daily encrypted database backup, hourly log summary, log rotation, 15-minute disk alert, certificate expiry check, backup integrity/restore test and package/security update report. Cron examples are in TaskB1/cron.example.

**Alternative – Ansible:** shell script is fast for one server but can drift and is often not idempotent. Ansible playbooks describe desired state, use modules, templates and inventory; setup cost is higher initially but applying to 20 servers is repeatable. For one server, shell may be quicker; for 20, Ansible reduces configuration drift and manual effort. Maintain playbooks, secrets and versioned inventories.

## A3 – Evaluate virtualisation (5 marks)

### A3.1 Hypervisors
Type-1 hypervisors run on hardware (KVM with Linux host architecture, ESXi, Xen, Hyper-V); type-2 run as applications on a host OS (VirtualBox, VMware Workstation). Full virtualisation presents virtual hardware so an unmodified guest OS runs; para-virtualisation uses guest awareness/hypercalls for selected operations. Intel VT-x and AMD-V provide CPU virtualisation support; IOMMU assists device assignment.

### A3.2 Benefits and costs
VMs isolate guest kernels, consolidate workloads, support snapshots and migration, and allow different OS versions. Costs include guest OS RAM/disk, boot time, hypervisor overhead, patching and commercial licences. For the university, use VMs for strong kernel/OS isolation and legacy workloads; run multiple app containers within a smaller VM cluster for density.

**Alternative – VirtualBox type-2 vs KVM type-1:** type-2 adds host OS scheduling and device layers and is suitable for developer/test labs; KVM is integrated with Linux and better suited to production. CPU-bound overhead may be low with hardware virtualisation; I/O overhead depends on virtio/paravirtual drivers, storage/network stack and host contention. Measure with the same workload and resource limits; do not invent a fixed percentage.

## A4 – Evaluate containerisation (4 marks)

### A4.1 Mechanisms
Namespaces isolate process IDs, mounts, network, IPC, users and hostnames. Cgroups account for and limit CPU, memory, I/O and process counts. Container images are layered read-only filesystem changes; a running container adds a writable layer. Images share common base layers on one host, reducing duplicate storage and start-up work.

### A4.2 Containers vs VMs
Containers share host kernel and start quickly with low memory overhead; kernel isolation is weaker and host-kernel vulnerabilities affect containers. VMs boot a guest kernel, consume more memory and take longer, but provide stronger kernel boundary. Use containers for stateless portal/LMS services; use VMs for database/legacy services needing separate kernel or stronger isolation, and protect data with network/storage controls.

**Alternative – portal and DB each in a VM:** each guest includes a full OS and independent kernel, improving fault/security isolation but increasing RAM/disk and start-up time (typically minutes rather than container seconds). Containers are more resource efficient, but the database needs durable volume, backup and stricter network access regardless of packaging.

## A5 – Cloud and orchestration (4 marks)

### A5.1 Service models and architecture
IaaS supplies VMs/network/storage; PaaS manages runtime/platform; SaaS is a finished application. For result day, use a hybrid design: private environment for steady baseline and sensitive systems, public cloud managed Kubernetes/autoscaling for burst portal replicas, load balancer/WAF, managed highly available database, object storage for static assets and CDN. Encrypt in transit/at rest, private subnets, least-privilege IAM, backups and tested restore.

### A5.2 Kubernetes
A Pod is the smallest deployable unit; a Deployment maintains desired replicas and replaces failed Pods; a Service provides stable discovery/load balancing. Requests guide scheduling; limits constrain consumption. HPA adjusts replica count from CPU/memory/custom metrics. Readiness probes remove unready Pods from service; liveness probes restart unhealthy containers. Replicas and health checks improve availability but do not replace database HA or backups.

**Alternative – fixed VMs:** pre-sized machines may handle known peak but sit idle most of the year, waste power/licence/capacity and need manual scaling. Autoscaling responds to demand and can reduce idle cost, but needs load tests, safe max replicas, quotas, warm-up and database capacity planning.

# PART B – IMPLEMENTATION & EXPERIMENTAL INVESTIGATION (25 marks)

## B1 – Administer Linux server (7 marks)

### B1.1 Observe
Run and save outputs:
```bash
ps aux --sort=-%cpu | head
top -b -n 1 | head -20
free -h
vmstat 1 5
iostat -xz 1 5          # install sysstat if missing
df -hT
ss -tulpn
```
Interpret CPU process use, load average, available memory, swap activity, block I/O wait/utilisation, filesystem capacity and listening sockets. Include date, machine specs and command output.

### B1.2 Automation
`TaskB1/log_summary.sh` uses awk/sort/uniq to count HTTP status codes and find the busiest client. Its sample fixture should report 200=4, 302=1, 403=1, 404=1; busiest client 10.0.0.5 with 4 requests; total bytes 43200. `disk_alert.sh` checks a filesystem threshold. Use cron.example after replacing placeholders with absolute paths. Test alert branch with a deliberately low threshold (e.g. 1) rather than filling the disk.

### B1.3 Service/network
Install app in /opt/result-portal, create systemd unit running Gunicorn as non-root `portal`, configure Nginx upstream proxy, use UFW default deny with 22/80/443 allowed (SSH source-restricted), then test `systemctl status`, `journalctl -u result-portal`, `curl -I http://localhost`, `nginx -t`, `ufw status verbose`. Do not enable firewall until SSH allow rule is confirmed.

### B1.4 Compare
The design separates proxy, app and DB identities and restricts ports. A single-VM lab implementation is a proof of configuration, not high availability; production needs multiple app replicas and resilient database.

**Alternative – central logs:** journald forwarding/ELK lets operators search across 10 servers centrally and correlate requests. It consumes extra storage and RAM for ingestion/indexing and requires certificates, retention, dashboards and access control. A local awk script is cheaper/simple for one host but cannot correlate distributed traces efficiently.

## B2 – Containerise portal (5 marks)

Files in TaskB2 include Dockerfile, Flask app, requirements and Compose. The Dockerfile uses python:3.12-slim, installs pinned dependencies, copies app, runs as non-root `portal`, exposes 8000 and launches Gunicorn. Build/run:
```bash
docker build -t result-portal .
docker run -d --name portal -p 8000:8000 --cpus=0.50 --memory=512m result-portal
docker ps
curl http://localhost:8000/
docker exec portal whoami
docker stats --no-stream portal
docker history result-portal
docker images result-portal
```
Expected identity: `portal`; health endpoint returns JSON healthy. Record actual image size and resource usage. Compose adds PostgreSQL with named volume and a separate Nginx LMS demo service; run `docker compose up -d --build`, `docker compose ps`, `docker compose logs`, `docker compose down` (do not add -v if you want to preserve database data). The sample Flask app does not yet perform database queries; Compose demonstrates service wiring and persistent database storage, not a complete DB-backed portal.

**Alternative – full Ubuntu base/multi-stage:** full Ubuntu typically increases image layers and packages, pull time and attack surface; slim base reduces these. Multi-stage builds are useful when compiling native dependencies, leaving build tools out of runtime. Measure image bytes with `docker image inspect`, build duration and pull duration on the same network.

## B3 – Compare bare metal, VM and container (8 marks)

### B3.1 Deploy three ways
Use same Flask/Gunicorn app and same endpoint. Native Linux: create venv and run Gunicorn. VM: install same OS/runtime in VirtualBox/KVM guest with fixed vCPU/RAM. Container: use TaskB2 Docker image. Record exact app version, Python/Gunicorn versions and resource limits.

### B3.2 Measure
For each method measure cold start (stop then start, timestamp), steady-state memory (RSS/PSS or VM process/guest memory), disk/image size and requests/second with identical concurrency:
`hey -n 10000 -c 100 http://HOST:PORT/health` or `ab -n 10000 -c 100 URL`.
Run at least 3 trials, report median, p95 latency, errors and machine configuration. Do not compare different hardware/CPU allocations as if controlled. Fill:
| Metric | Native Linux | VM | Docker |
|---|---:|---:|---:|
| Start-up (s) | measure | measure | measure |
| Memory (MiB) | measure | measure | measure |
| Disk/image (MiB) | measure | measure | measure |
| Requests/s | measure | measure | measure |
| p95 latency (ms) | measure | measure | measure |

### B3.3 Kubernetes
Build image, load it into minikube/kind or push to a registry accessible to cluster; set image reference. Apply TaskB3/deployment.yaml. Verify:
`kubectl get pods -o wide`
`kubectl describe pod <pod>`
`kubectl delete pod <pod>`
`kubectl get pods -w`
`kubectl scale deployment/result-portal --replicas=6`
The Deployment controller should recreate a deleted Pod and maintain desired replicas. Use `kubectl top pods` after metrics-server is available. HPA requires metrics-server and a configured HPA object; static replica scaling is not autoscaling.

### B3.4 Trade-off
Native Linux has least packaging overhead but weaker portability/reproducibility. VM offers guest-kernel isolation and predictable OS but higher memory/start time. Container gives fast rollout and density, but shares host kernel. Kubernetes adds scheduling, service discovery and self-healing at operational complexity. Compose scales containers on one host; it cannot provide multi-node scheduling or recover from loss of that host. Kubernetes can reschedule onto another healthy node if capacity and storage/network dependencies permit.

## B4 – Final engineering decision (5 marks)

### B4.1 Second service
TaskB2 Compose includes portal, PostgreSQL and LMS demo with CPU/memory limits. TaskB3 Kubernetes manifest includes result-portal (3 replicas) and lms-demo (2 replicas), each with requests and limits. Apply and inspect pods; capture outputs.

### B4.2 Capacity estimate
150,000 students over 2 hours gives average login arrival rate:
(150,000/(2×3600)=20.83) students/second.
This is not the HTTP request rate. If each login causes (r) requests, average arrival rate is (20.83r) requests/s. If one tested pod sustains (q) requests/s at the required p95 latency, with target utilisation (u) (e.g. 0.6), estimated replicas (=lceil(20.83r)/(q×u)×safetyFactorceil). Add burst factor from the observed peak profile, not just the two-hour average. Database connection pool, cache, login burst and static asset CDN may be bottlenecks before app CPU.

### B4.3 Decision
Recommend hybrid: retain private infrastructure for baseline and controlled data, burst stateless portal replicas to public cloud Kubernetes behind WAF/load balancer, and use a managed HA database with private connectivity and tested backup/restore. This avoids buying peak-only servers while maintaining control over core records. Confirm data residency, privacy contract, egress and per-semester cost. A pure public-cloud design can add capacity quickly but annual cost includes always-on baseline, two result-day bursts, storage, backups, requests and data egress. A precise INR estimate needs provider, region, VM/node size, DB tier, traffic and storage assumptions; do not invent a price.

## Required evidence before final submission
- Actual Linux outputs for B1 commands, systemd/Nginx/UFW status and cron logs.
- Docker build output, `docker ps`, `docker stats`, `docker history`, `docker images`, `whoami`.
- VM/native/container measurements under identical conditions.
- Kubernetes 3 pods Running, delete/recreate evidence, scale to 6, describe resources and service endpoint.
- Record date, host specs, versions, command line and screenshot for each experiment.
- Replace every placeholder with your real measured value and add screenshot files to the matching task folder.
