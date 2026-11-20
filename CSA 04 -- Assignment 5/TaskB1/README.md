# B1 – Linux server administration

Run observation commands and save outputs:
```bash
date -Is
uname -a
lsb_release -a
ps aux --sort=-%cpu | head
top -b -n 1 | head -20
free -h
vmstat 1 5
iostat -xz 1 5       # requires sysstat
df -hT
ss -tulpn
```

Run scripts:
```bash
bash log_summary.sh
bash disk_alert.sh 80 /
bash disk_alert.sh 1 /   # safely exercises the alert branch
```

Install service and Nginx configs only after adapting paths and creating the `portal` service account:
```bash
sudo install -m 0644 result-portal.service /etc/systemd/system/result-portal.service
sudo install -m 0644 nginx-result-portal.conf /etc/nginx/sites-available/result-portal
sudo ln -s /etc/nginx/sites-available/result-portal /etc/nginx/sites-enabled/result-portal
sudo nginx -t
sudo systemctl daemon-reload
sudo systemctl enable --now result-portal nginx
sudo systemctl status result-portal nginx
```
Before enabling UFW, permit SSH from your trusted admin network to avoid locking yourself out. Never expose Gunicorn port 8000 or the database directly to the internet.
