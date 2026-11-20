#define _GNU_SOURCE
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static void show_maps(void) {
    FILE *f=fopen("/proc/self/maps","r");
    if(!f){perror("fopen maps");return;}
    char line[512];
    puts("Mapped executable/shared-library regions:");
    while(fgets(line,sizeof line,f))
        if(strstr(line,"link_demo") || strstr(line,"libcrypto_plugin")) fputs(line,stdout);
    fclose(f);
}
int main(void) {
    printf("Main function address: %p\n", (void *)&main);
    void *h = dlopen("./libcrypto_plugin.so", RTLD_NOW);
    if (!h) { fprintf(stderr, "dlopen failed: %s\n", dlerror()); return EXIT_FAILURE; }
    dlerror();
    const char *(*version)(void) = (const char *(*)(void))dlsym(h, "crypto_version");
    const char *e = dlerror();
    if (e) { fprintf(stderr, "dlsym crypto_version failed: %s\n", e); dlclose(h); return EXIT_FAILURE; }
    dlerror();
    void (*run)(void) = (void (*)(void))dlsym(h, "crypto_transform");
    e = dlerror();
    if (e) { fprintf(stderr, "dlsym crypto_transform failed: %s\n", e); dlclose(h); return EXIT_FAILURE; }
    printf("Loaded library version: %s\n", version());
    run();
    show_maps();
    dlclose(h);
    return 0;
}
