#include "enc.h"
int encrypt_version(void) { return 1; }
void encrypt_buf(char *s) { for (; *s; s++) *s ^= 0x2A; }
