#include <stdio.h>
#include <dlfcn.h>
int main(void) {
    void *h = dlopen("./libenc.so", RTLD_NOW);
    if (!h) { printf("dlopen failed: %s\n(graceful fallback: encryption disabled)\n", dlerror()); return 1; }
    int (*ver)(void) = (int (*)(void))dlsym(h, "encrypt_version");
    void (*enc)(char *) = (void (*)(char *))dlsym(h, "encrypt_buf");
    if (!ver || !enc) { printf("dlsym failed: %s\n", dlerror()); return 1; }
    char m[] = "bank-record"; enc(m);
    printf("loaded at run time: version %d, byte0 0x%02x\n", ver(), (unsigned char)m[0]);
    dlclose(h); return 0;
}
