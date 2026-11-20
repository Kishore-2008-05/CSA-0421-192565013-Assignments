# B3 – Native, VM, container and Kubernetes comparison

## Local Kubernetes setup
The manifest references `simats/result-portal:1.0`. Build the image with that exact tag and load it into minikube, or push it to a registry accessible to the cluster:
```bash
cd ../TaskB2
docker build -t simats/result-portal:1.0 .
minikube image load simats/result-portal:1.0
cd ../TaskB3
kubectl apply -f deployment.yaml
kubectl get pods -o wide
kubectl get svc
kubectl describe pod <pod-name>
```
Delete one portal pod and watch the Deployment replace it:
```bash
kubectl delete pod <portal-pod-name>
kubectl get pods -w
```
Scale to six replicas:
```bash
kubectl scale deployment/result-portal --replicas=6
kubectl get deployment result-portal
```
For HPA, enable metrics-server first, apply `hpa.yaml`, then generate load:
```bash
minikube addons enable metrics-server
kubectl apply -f hpa.yaml
kubectl get hpa -w
bash load_test.sh http://127.0.0.1:8000/health
```
Use port-forward if needed: `kubectl port-forward svc/result-portal 8000:80`.

## Required comparison
Deploy identical app version on native Linux, a VM and Docker with matching CPU/memory. Run at least three trials with same `hey`/ `ab` request count and concurrency. Record cold-start time, RSS/PSS or guest memory, disk/image size, requests/s, p95 latency and errors. Keep raw logs and screenshots. A successful YAML apply alone is not evidence of a running/healthy service.
