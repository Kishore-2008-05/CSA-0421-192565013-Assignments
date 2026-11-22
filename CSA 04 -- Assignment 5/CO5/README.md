# CO5 – Linux Administration, Containers and Virtualisation (Register No. 192565013)
Sandbox: gcc/Python on Ubuntu 24.04.5, Linux 6.18 (replace with your own system details when you re-run).

## What was actually run vs. only written
| Item | Status |
|---|---|
| B1 `log_summary.sh` (TODO 1–2) | Run; output matches expected (`B1/log_summary_output.txt`) |
| B1 `disk_alert.sh`, bash syntax of all scripts | Run / syntax-checked |
| B1 systemd unit, nginx conf, ufw script, crontab | **Written, not run** (no systemd/nginx/ufw/root here) |
| B2 Dockerfile (TODO 1–3), Compose file | **Written, not built or run** (no Docker here) |
| B3 `deployment.yaml` (TODO 1–2), B4 `lms-service.yaml` | YAML syntax validated only; **not applied** (no cluster) |
| B3 native measurement | Run in sandbox (`B3/native_measurements.txt`) |
| VM, Docker, Kubernetes measurements, `docker stats`, `docker history`, pod deletion/scaling | **Not done – you must run these** (commands below) |

## Commands for you to run (WSL2 + Docker Desktop, minikube or kind)
```bash
cd B2 && docker build -t result-portal . && docker run -d -p 8000:8000 --name rp result-portal
docker ps; docker exec rp whoami; curl localhost:8000; docker images result-portal; docker history result-portal
docker run -d --cpus 0.5 --memory 256m -p 8001:8000 result-portal; docker stats --no-stream
docker compose up -d
cd ../B3 && kubectl apply -f deployment.yaml && kubectl get pods
kubectl delete pod <name>; kubectl get pods; kubectl scale deployment result-portal --replicas=6
kubectl describe pod <name> | grep -A4 -E "Limits|Requests"
cd ../B4 && kubectl apply -f lms-service.yaml
```
(Note: the Dockerfile adds the non-root `portal` user and `USER portal`; for minikube load the image with `minikube image load result-portal` and use image name `simats/result-portal:1.0` via `docker tag`.)

## Native measurement (sandbox, not your machine)
Startup 0.18 s; 1,568 requests/s with the Flask development server on loopback with a Python client (not a production benchmark); server RSS ≈ 32 MB. Use `ab` or `wrk` for yours.

## B4 capacity estimate (`B4/capacity_estimate.txt`)
150,000 students in 2 hours with 5 requests each ≈ 104 req/s average; with a 5× peak ≈ 521 req/s. The measured 1,568 req/s is far too optimistic for the real app, so I assumed 10% (157 req/s per container) → 4 containers (+1 spare). **These are assumptions – replace them with your own measured value.**

## Part A summary (expand each in your report)
- **A1 Linux:** kernel (process, memory, VFS, device, network subsystems) + libraries (glibc) + shell/utilities. CFS gives each runnable task a fair share using virtual runtime in a red-black tree, O(log n) pick. Page cache keeps file blocks in RAM; swap holds cold anonymous pages; `/proc` and `/sys` expose kernel state. Permissions: owner/group/other rwx, setuid runs with the file owner's rights, sudo grants controlled root. *Alternative Windows Server:* larger base footprint, licence cost per server, PowerShell automation.
- **A2 Admin:** a `portal` service user, an `ops` group, SSH keys only, systemd unit with `Restart=on-failure`, journald/log scripts, Nginx reverse proxy, ufw allowing 22/80/443, cron for backups, log rotation and disk alerts. *Alternative Ansible:* setup time grows O(1) per extra server versus O(n) manual scripting, and avoids configuration drift.
- **A3 Virtualisation:** type 1 (KVM, ESXi, Xen, Hyper-V) runs on hardware; type 2 (VirtualBox) runs on a host OS. VT-x/AMD-V make full virtualisation efficient; para-virtualisation uses guest-aware drivers. Benefits: isolation, consolidation, snapshots, live migration; costs: guest OS memory and CPU overhead, licences. *Alternative type 2:* more overhead and a host OS in the path – fine for testing, not production.
- **A4 Containers:** namespaces isolate what a process sees (pid, net, mnt, uts, ipc, user); cgroups limit what it uses (CPU, memory, I/O); images are read-only layers plus a writable layer. Containers start in seconds and share the host kernel; VMs take minutes and give stronger isolation.
- **A5 Cloud/K8s:** IaaS (VMs), PaaS (platform), SaaS (software). Result-day design: managed Kubernetes with an autoscaler, a load balancer and a managed database. Pods run containers; a Deployment keeps N replicas and recreates failed pods; a Service gives a stable address; requests/limits drive scheduling; the HPA adds pods under load. *Alternative fixed VMs:* sized for the peak, so idle 364 days a year.
- **B4 decision (reasoned, not yet proven by measurement):** hybrid – run steady load on the university's own Kubernetes cluster and burst to the public cloud for result day; keep student data on university-controlled storage. Cost figures must be added from current provider pricing, which I have not looked up.

## Not yet written
Full diagrams/flowcharts for Part A, the per-task READMEs, Big-O write-ups for each alternative, B1.4 comparison with the design, and a cost table.
