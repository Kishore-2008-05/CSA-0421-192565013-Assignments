#define _GNU_SOURCE
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
int main(void) {
    printf("Main function address: %p\n", (void *)&main);
    void *h = dlopen("./libcrypto_plugin.so", RTLD_NOW);
    if (!h) { fprintf(stderr, "dlopen failed: %s\n", dlerror()); return EXIT_FAILURE; }
    const char *(*version)(void) = (const char *(*)(void))dlsym(h, "crypto_version");
    void (*run)(void) = (void (*)(void))dlsym(h, "crypto_transform");
    const char *e = dlerror();
    if (e) { fprintf(stderr, "dlsym failed: %s\n", e); dlclose(h); return EXIT_FAILURE; }
    printf("Loaded library version: %s\n", version());
    run();
    dlclose(h);
    return 0;
}
