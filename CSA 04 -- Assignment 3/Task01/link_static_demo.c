#include <stdio.h>
const char *crypto_version(void);
void crypto_transform(void);
int main(void){printf("Static/object-linked library version: %s\n",crypto_version());crypto_transform();return 0;}
