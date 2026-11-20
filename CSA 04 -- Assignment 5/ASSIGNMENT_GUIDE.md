# CO5 – Linux, Containers and Virtualisation | Register 192565013

Student: Kishore Kumar S

## Implemented
- TaskB1: awk/sort/uniq log summary script and captured output.
- TaskB2: Flask health-check application, requirements and Dockerfile using python:3.12-slim, a non-root portal user and Gunicorn on port 8000.
- TaskB3: Kubernetes Deployment and Service with three replicas, CPU/memory requests and limits, and readiness probe.

## Run
```bash
bash TaskB1/log_summary.sh
cd TaskB2
docker build -t result-portal .
docker run --rm -p 8000:8000 result-portal
```
Publish the image referenced by the manifest before applying:
```bash
kubectl apply -f TaskB3/deployment.yaml
kubectl get pods
```

B1 was executed in a Linux test container. Docker build/run and Kubernetes deployment have not been run here; capture real outputs and screenshots on your own Linux host/cluster. Record `uname -a`, `lsb_release -a`, `docker --version` and `kubectl version --client`.

## Still required
CO5 Part A design, B1 system observation/cron/service evidence, B2 resource limits and image optimisation, B3 deployment measurements and recovery tests, and B4 capacity/production decision remain. This is not yet a complete 50-mark submission.