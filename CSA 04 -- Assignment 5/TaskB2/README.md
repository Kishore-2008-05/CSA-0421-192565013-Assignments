# B2 – Containerise the result portal
```bash
docker build -t result-portal .
docker run -d --name portal -p 8000:8000 --cpus=0.50 --memory=512m result-portal
curl http://localhost:8000/health
docker ps
docker exec portal whoami
docker stats --no-stream portal
docker history result-portal
docker images result-portal
```
Expected service user is `portal`. Compose adds PostgreSQL with a named persistent volume and an LMS demo container. The sample Flask app is a health-check skeleton; it is not a complete database-backed student-results system. Record actual image size and runtime statistics on the machine where Docker is installed.