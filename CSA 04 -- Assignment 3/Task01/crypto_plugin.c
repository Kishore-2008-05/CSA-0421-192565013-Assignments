#include <stdio.h>
const char *crypto_version(void) { return "crypto-plugin-v1"; }
void crypto_transform(void) { puts("Sample transform executed by shared library."); }
