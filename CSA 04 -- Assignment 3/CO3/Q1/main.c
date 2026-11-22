#include <stdio.h>
#include <string.h>
#include "enc.h"
int main(void) {
    char m[] = "bank-record";
    encrypt_buf(m);
    printf("library version %d, first byte of ciphertext 0x%02x\n", encrypt_version(), (unsigned char)m[0]);
    FILE *f = fopen("/proc/self/maps", "r"); char l[300];
    while (f && fgets(l, sizeof l, f)) if (strstr(l, "main") || strstr(l, "enc") || strstr(l, "libc.so") || strstr(l, "[stack]")) fputs(l, stdout);
    if (f) fclose(f);
    return 0;
}
