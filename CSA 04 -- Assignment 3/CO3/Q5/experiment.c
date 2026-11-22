#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define MAXF 10
#define PAGES 50

static int find(const int f[], int n, int p) { for (int i = 0; i < n; i++) if (f[i] == p) return i; return -1; }

static int fifo(const int *r, int len, int fr) {
    int f[MAXF], used = 0, next = 0, faults = 0;
    for (int i = 0; i < len; i++) {
        if (find(f, used, r[i]) >= 0) continue;
        faults++;
        if (used < fr) f[used++] = r[i]; else { f[next] = r[i]; next = (next + 1) % fr; }
    }
    return faults;
}
static int lru(const int *r, int len, int fr) {
    int f[MAXF], last[MAXF], used = 0, faults = 0;
    for (int i = 0; i < len; i++) {
        int k = find(f, used, r[i]);
        if (k >= 0) { last[k] = i; continue; }
        faults++;
        if (used < fr) { f[used] = r[i]; last[used++] = i; }
        else { int v = 0; for (int j = 1; j < fr; j++) if (last[j] < last[v]) v = j; f[v] = r[i]; last[v] = i; }
    }
    return faults;
}
/* Optimal using precomputed next-use array: O(len*frames) */
static int optimal(const int *r, int len, int fr) {
    int *nu = malloc(len * sizeof *nu), pos[PAGES];
    for (int p = 0; p < PAGES; p++) pos[p] = len;
    for (int i = len - 1; i >= 0; i--) { nu[i] = pos[r[i]]; pos[r[i]] = i; }
    int f[MAXF], nxt[MAXF], used = 0, faults = 0;
    for (int i = 0; i < len; i++) {
        int k = find(f, used, r[i]);
        if (k >= 0) { nxt[k] = nu[i]; continue; }
        faults++;
        if (used < fr) { f[used] = r[i]; nxt[used++] = nu[i]; }
        else { int v = 0; for (int j = 1; j < fr; j++) if (nxt[j] > nxt[v]) v = j; f[v] = r[i]; nxt[v] = nu[i]; }
    }
    free(nu); return faults;
}
static int clock_alg(const int *r, int len, int fr) {
    int f[MAXF], ref[MAXF] = {0}, used = 0, hand = 0, faults = 0;
    for (int i = 0; i < len; i++) {
        int k = find(f, used, r[i]);
        if (k >= 0) { ref[k] = 1; continue; }
        faults++;
        if (used < fr) { f[used] = r[i]; ref[used++] = 1; continue; }
        while (ref[hand]) { ref[hand] = 0; hand = (hand + 1) % fr; }
        f[hand] = r[i]; ref[hand] = 1; hand = (hand + 1) % fr;
    }
    return faults;
}
/* Strong locality: working set of 6 pages that drifts; 90% refs inside it */
static void gen_locality(int *r, int len) {
    int base = 0;
    for (int i = 0; i < len; i++) {
        if (i % 500 == 0) base = rand() % (PAGES - 6);
        r[i] = (rand() % 100 < 90) ? base + rand() % 6 : rand() % PAGES;
    }
}
static void gen_random(int *r, int len) { for (int i = 0; i < len; i++) r[i] = rand() % PAGES; }
static double ms(clock_t a, clock_t b) { return 1000.0 * (b - a) / CLOCKS_PER_SEC; }

int main(void) {
    const int LEN = 100000;
    int *r = malloc(LEN * sizeof *r);
    FILE *csv = fopen("results.csv", "w");
    fprintf(csv, "workload,frames,FIFO,LRU,OPT,CLOCK\n");
    const char *names[] = {"locality", "random"};
    for (int w = 0; w < 2; w++) {
        srand(12345);
        if (w == 0) gen_locality(r, LEN); else gen_random(r, LEN);
        for (int fr = 3; fr <= 10; fr++)
            fprintf(csv, "%s,%d,%d,%d,%d,%d\n", names[w], fr, fifo(r, LEN, fr), lru(r, LEN, fr), optimal(r, LEN, fr), clock_alg(r, LEN, fr));
    }
    fclose(csv);
    /* timing, 100,000 references, 8 frames, locality workload, 200 repeats */
    srand(12345); gen_locality(r, LEN);
    int (*alg[])(const int *, int, int) = {fifo, lru, optimal, clock_alg};
    const char *an[] = {"FIFO", "LRU", "OPT", "CLOCK"};
    volatile int sink = 0; const int REP = 200;
    printf("Time for %d references, 8 frames (average of %d runs)\n", LEN, REP);
    for (int a = 0; a < 4; a++) {
        clock_t t0 = clock();
        for (int k = 0; k < REP; k++) sink += alg[a](r, LEN, 8);
        printf("%-6s %.3f ms\n", an[a], ms(t0, clock()) / REP);
    }
    (void)sink; free(r); return 0;
}
