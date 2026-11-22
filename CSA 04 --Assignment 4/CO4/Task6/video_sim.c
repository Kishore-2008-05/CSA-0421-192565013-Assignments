/* Task 6 model-based simulation: two storage designs for lecture-video streaming.
   Design A: HDD only, FCFS.  Design B: SSD cache (LRU, whole videos) + HDD with batched C-SCAN.
   Device times are ASSUMED textbook figures (see README), not measurements of real hardware. */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define V 2000            /* videos, 1 GB average -> 2 TB library */
#define REQS 200000       /* random "jump to any minute" chunk requests */
#define CHUNK_MB 4.0
static double hdd_seek(double d) { return 2.0 + 12.0 * d; }       /* ms, d = fraction of disk crossed */
static double hdd_xfer(void) { return CHUNK_MB / 150.0 * 1000.0; } /* 150 MB/s */
#define HDD_ROT 4.17                                                /* ms, 7200 rpm average */
static double ssd_time(void) { return 0.1 + CHUNK_MB / 2000.0 * 1000.0; } /* 2 GB/s NVMe */
static double cdf[V];
static int cmpd(const void *a, const void *b) { double x = *(const double *)a, y = *(const double *)b; return (x > y) - (x < y); }
static int zipf_pick(void) { double u = rand() / (RAND_MAX + 1.0); int lo = 0, hi = V - 1; while (lo < hi) { int m = (lo + hi) / 2; if (cdf[m] < u) lo = m + 1; else hi = m; } return lo; }
int main(void) {
    double s = 0; for (int i = 0; i < V; i++) { s += 1.0 / (i + 1); cdf[i] = s; } for (int i = 0; i < V; i++) cdf[i] /= s;
    srand(99);
    static int req[REQS]; static double pos[REQS];
    for (int i = 0; i < REQS; i++) { req[i] = zipf_pick(); pos[i] = (req[i] * 7919 % V + rand() / (RAND_MAX + 1.0)) / V; } /* scatter videos over the disk */
    /* Design A */
    double head = 0.5, tA = 0;
    for (int i = 0; i < REQS; i++) { tA += hdd_seek(fabs(pos[i] - head)) + HDD_ROT + hdd_xfer(); head = pos[i]; }
    double avgA = tA / REQS;
    printf("Design A (HDD only, FCFS): avg service %.2f ms -> %.1f requests/s per HDD\n", avgA, 1000.0 / avgA);
    /* Design B: SSD cache sizes */
    int sizes[] = {100, 200, 400};
    for (int c = 0; c < 3; c++) {
        int csz = sizes[c]; static long last[V]; static int in[V]; int cnt = 0; long hits = 0;
        for (int i = 0; i < V; i++) { in[i] = 0; last[i] = 0; }
        static double miss[REQS]; int nm = 0;
        for (int i = 0; i < REQS; i++) {
            int v = req[i];
            if (in[v]) { hits++; last[v] = i + 1; continue; }
            miss[nm++] = pos[i];
            if (cnt >= csz) { int vic = -1; for (int j = 0; j < V; j++) if (in[j] && (vic < 0 || last[j] < last[vic])) vic = j; in[vic] = 0; cnt--; }
            in[v] = 1; last[v] = i + 1; cnt++;
        }
        /* HDD misses served in batches of 32, sorted (C-SCAN style) */
        double th = 0; head = 0.5;
        for (int b = 0; b < nm; b += 32) { int n = nm - b < 32 ? nm - b : 32; qsort(miss + b, n, sizeof(double), cmpd);
            for (int k = 0; k < n; k++) { th += hdd_seek(fabs(miss[b + k] - head)) + HDD_ROT + hdd_xfer(); head = miss[b + k]; } }
        double h = (double)hits / REQS, avgHdd = nm ? th / nm : 0, avgAll = (hits * ssd_time() + th) / REQS;
        printf("Design B (SSD cache %d videos = %.0f%% of library, batched C-SCAN HDD): hit ratio %.1f%%, HDD avg %.2f ms (%.1f req/s), overall avg %.2f ms\n",
               csz, 100.0 * csz / V, 100 * h, avgHdd, avgHdd ? 1000 / avgHdd : 0, avgAll);
        /* devices needed for 937 req/s peak (10,000 streams x one 4 MB chunk / 10.7 s) */
        double peak = 937.0, hddReq = peak * (1 - h);
        printf("   peak %.0f req/s: HDDs needed %.0f (A needs %.0f); SSD load %.0f req/s of ~%.0f/s per NVMe\n",
               peak, ceil(hddReq / (avgHdd ? 1000 / avgHdd : 1)), ceil(peak / (1000.0 / avgA)), peak * h, 1000.0 / ssd_time());
    }
    return 0;
}
