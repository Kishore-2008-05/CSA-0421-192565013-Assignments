import subprocess, time, urllib.request, threading, sys, resource, os
t0=time.time()
p=subprocess.Popen([sys.executable,"-c","from app import app; app.run(port=8765,threaded=True)"],cwd="../B2",stdout=subprocess.DEVNULL,stderr=subprocess.DEVNULL)
while True:
    try: urllib.request.urlopen("http://127.0.0.1:8765/health").read(); break
    except Exception: time.sleep(0.01)
print(f"startup to first successful /health: {time.time()-t0:.2f} s")
N=2000; W=8; done=[0]
def worker():
    for _ in range(N//W): urllib.request.urlopen("http://127.0.0.1:8765/health").read(); done[0]+=1
t=time.time(); th=[threading.Thread(target=worker) for _ in range(W)]
[x.start() for x in th]; [x.join() for x in th]
el=time.time()-t; print(f"{done[0]} requests, {W} threads: {done[0]/el:.0f} requests/s (Flask dev server, loopback, python client)")
rss=[l for l in open(f"/proc/{p.pid}/status") if l.startswith("VmRSS")][0].split()[1]
print(f"server RSS: {rss} kB"); p.terminate()
