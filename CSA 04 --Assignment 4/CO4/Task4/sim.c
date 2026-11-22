/* Disk-scheduling simulation. Cylinders 0-199. Time unit = 1 per cylinder moved,
   +1 per request for rotation/transfer. Requests arrive by a Poisson-like process. */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define CYL 200
#define MAXQ 20000
enum {FCFS, SSTF, SCAN, CSCAN, LOOK, CLOOK, NALG};
static const char *nm[] = {"FCFS", "SSTF", "SCAN", "C-SCAN", "LOOK", "C-LOOK"};
typedef struct { int cyl; double arr; } Req;

/* pick index of next request in pending[] (np entries) */
static int pick(int alg, Req *q, int np, int head, int *dir) {
    int best = -1;
    if (alg == FCFS) { for (int i = 0; i < np; i++) if (best < 0 || q[i].arr < q[best].arr) best = i; return best; }
    if (alg == SSTF) { for (int i = 0; i < np; i++) if (best < 0 || abs(q[i].cyl - head) < abs(q[best].cyl - head)) best = i; return best; }
    if (alg == SCAN || alg == LOOK) {
        for (int pass = 0; pass < 2; pass++) {
            for (int i = 0; i < np; i++) {
                int d = (q[i].cyl - head) * *dir;
                if (d >= 0 && (best < 0 || d < (q[best].cyl - head) * *dir)) best = i;
            }
            if (best >= 0) return best;
            *dir = -*dir; /* nothing ahead: reverse */
        }
        return best;
    }
    /* C-SCAN / C-LOOK: always upward, wrap to lowest */
    for (int i = 0; i < np; i++) if (q[i].cyl >= head && (best < 0 || q[i].cyl < q[best].cyl)) best = i;
    if (best >= 0) return best;
    for (int i = 0; i < np; i++) if (best < 0 || q[i].cyl < q[best].cyl) best = i;
    return best;
}
/* head travel between head and target for this algorithm (SCAN/C-SCAN go to disk ends) */
static double travel(int alg, int head, int tgt, int dir_before, int dir_after) {
    if (alg == SCAN && dir_before != dir_after) { int end = dir_before > 0 ? CYL - 1 : 0; return abs(end - head) + abs(tgt - end); }
    if (alg == CSCAN && tgt < head) return (CYL - 1 - head) + (CYL - 1) + tgt; /* run to end, jump to 0 (counted as full sweep), seek up */
    return abs(tgt - head);
}
static void run(int alg, double mean_ia, int n, unsigned seed, double *avgmove, double *maxwait, double *avgwait) {
    srand(seed);
    static Req all[MAXQ], pend[MAXQ];
    double t = 0;
    for (int i = 0; i < n; i++) { t += -mean_ia * log((rand() + 1.0) / (RAND_MAX + 2.0)); all[i].arr = t; all[i].cyl = rand() % CYL; }
    int np = 0, next = 0, head = 100, dir = 1, done = 0;
    double now = 0, moved = 0, mw = 0, sw = 0;
    while (done < n) {
        while (next < n && all[next].arr <= now) pend[np++] = all[next++];
        if (np == 0) { now = all[next].arr; continue; }
        int d0 = dir, k = pick(alg, pend, np, head, &dir);
        double mv = travel(alg, head, pend[k].cyl, d0, dir);
        if (alg == LOOK || alg == SCAN) { /* if direction flipped by pick, LOOK just reverses at last request */ }
        now += mv + 1; moved += mv;
        double w = now - pend[k].arr; sw += w; if (w > mw) mw = w;
        head = pend[k].cyl; pend[k] = pend[--np]; done++;
    }
    *avgmove = moved / n; *maxwait = mw; *avgwait = sw / n;
}
/* starvation: request at 183 present at t=0; new request near cylinder 60 every 1 time unit */
static void starve(int alg) {
    Req pend[MAXQ]; int np = 0, head = 53, dir = 1;
    pend[np].cyl = 183; pend[np++].arr = 0;
    double now = 0, nextarr = 0; long served = 0;
    srand(5);
    while (now < 100000) {
        while (nextarr <= now && np < MAXQ - 1) { pend[np].cyl = 58 + rand() % 5; pend[np++].arr = nextarr; nextarr += 1; }
        int d0 = dir, k = pick(alg, pend, np, head, &dir);
        double mv = travel(alg, head, pend[k].cyl, d0, dir);
        now += mv + 1; head = pend[k].cyl;
        if (pend[k].cyl == 183) { printf("  %-6s: request at 183 served after %.0f time units (%ld other requests first)\n", nm[alg], now, served); return; }
        pend[k] = pend[--np]; served++;
    }
    printf("  %-6s: request at 183 STARVED - still waiting after 100000 time units (%ld others served)\n", nm[alg], served);
}
int main(void) {
    double loads[] = {200, 50, 20}; const char *ln[] = {"light (mean inter-arrival 200)", "medium (50)", "heavy (20)"};
    printf("10,000 random requests, cylinders 0-199, seed 2024\n");
    for (int l = 0; l < 3; l++) {
        printf("\n%s\n%-7s %12s %12s %12s\n", ln[l], "alg", "avg move", "avg wait", "max wait");
        for (int a = 0; a < NALG; a++) { double am, mw, aw; run(a, loads[l], 10000, 2024, &am, &mw, &aw); printf("%-7s %12.2f %12.1f %12.1f\n", nm[a], am, aw, mw); }
    }
    printf("\nStarvation test: stream near cylinder 60 every 1 time unit\n");
    for (int a = 0; a < NALG; a++) starve(a);
    return 0;
}
