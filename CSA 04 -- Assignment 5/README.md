# CO5 – Linux, Containers and Virtualisation

**Student:** Kishore Kumar S  
**Register number:** 192565013  
**Compiler:** GCC (record your local `gcc --version`)  
**OS:** Ubuntu on WSL (record `lsb_release -a` and `uname -a`).

## Build and run the sample portal
```bash
cd TaskB2
docker build -t result-portal .
docker run --rm -p 8000:8000 result-portal
```
In another terminal: `curl http://localhost:8000/health`. Docker is required. Compose file includes portal, PostgreSQL persistent volume and an LMS demo service.

## Kubernetes
Build/push or load `simats/result-portal:1.0` into the local cluster, then:
```bash
kubectl apply -f TaskB3/deployment.yaml
kubectl get pods
kubectl describe pod <pod-name>
kubectl delete pod <pod-name>
kubectl get pods -w
kubectl scale deployment/result-portal --replicas=6
```

## Evidence
TaskB1 includes log summary and disk alert scripts plus cron examples. TaskB2 includes Dockerfile, app and Compose. TaskB3 includes portal/LMS deployments with CPU/memory limits and a load-test helper. The complete report contains design and experimental procedures. Docker/VM/Kubernetes tests and genuine screenshots must be collected on the student's own machine/cluster; they are not claimed as completed here.