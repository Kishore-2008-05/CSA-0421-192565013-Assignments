# Q1 – Linking, loading and address binding

```bash
# Static link of the application object and library object
gcc -std=c11 -Wall -Wextra -c crypto_plugin.c -o crypto_plugin.o
gcc -std=c11 -Wall -Wextra link_static_demo.c crypto_plugin.o -o link_static_demo
# Shared library and startup dynamic linking
gcc -std=c11 -Wall -Wextra -fPIC -shared crypto_plugin.c -o libcrypto_plugin.so
gcc -std=c11 -Wall -Wextra link_dynamic_demo.c -L. -lcrypto_plugin -Wl,-rpath,'$ORIGIN' -o link_dynamic_demo
# Runtime dlopen/dlsym demonstration
gcc -std=c11 -Wall -Wextra link_demo.c -o link_demo -ldl
./link_static_demo
./link_dynamic_demo
./link_demo
ldd ./link_dynamic_demo
stat -c '%n %s bytes' link_static_demo link_dynamic_demo
```

The dlopen program prints matching entries from `/proc/self/maps`. To test a missing library, temporarily rename `libcrypto_plugin.so` and run `./link_demo`; restore it immediately afterward. Measure elapsed time/RSS with `/usr/bin/time -v` and record multiple trials. These are sample functions, not production cryptography.
