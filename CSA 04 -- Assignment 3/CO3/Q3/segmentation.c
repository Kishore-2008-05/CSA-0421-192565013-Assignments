#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

typedef struct { unsigned base, limit; const char *name; } Seg;
static Seg st[] = {{1000, 400, "code"}, {6300, 400, "data"}, {4300, 1100, "stack"}, {3200, 1000, "heap"}};
#define NSEG 4

static void translate(unsigned s, unsigned off) {
    if (s >= NSEG) { printf("(%u,%u) -> invalid segment\n", s, off); return; }
    if (off >= st[s].limit)
        printf("(%u,%u) -> trap: offset %u >= limit %u (%s)\n", s, off, off, st[s].limit, st[s].name);
    else
        printf("(%u,%u) -> physical %u (%s)\n", s, off, st[s].base + off, st[s].name);
}
static void on_segv(int sig) {
    const char m[] = "SIGSEGV caught: kernel delivered signal 11 after the MMU raised an invalid-access fault\n";
    (void)sig; write(1, m, sizeof m - 1); _exit(0);
}
int main(void) {
    unsigned t[8][2] = {{0,0},{0,399},{0,400},{1,50},{2,1099},{2,1100},{3,999},{4,0}};
    for (int i = 0; i < 8; i++) translate(t[i][0], t[i][1]);
    fflush(stdout); /* flush before the deliberate fault: handler uses _exit() */
    signal(SIGSEGV, on_segv);
    volatile int *p = NULL; *p = 1;
    return 0;
}
