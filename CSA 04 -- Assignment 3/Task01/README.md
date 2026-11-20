# Q1 – Linking, loading and address binding

Build the shared object and dynamically loaded program:

```bash
gcc -std=c11 -Wall -Wextra -fPIC -shared crypto_plugin.c -o libcrypto_plugin.so
gcc -std=c11 -Wall -Wextra link_demo.c -o link_demo -ldl
./link_demo
ldd ./link_demo
cat /proc/$(pgrep -n link_demo)/maps   # run while process is alive; use a pause/sleep build for inspection
```

For a static-vs-dynamic comparison, compile a separate object/library pair and compare `size` and `stat -c %s`. A static build embeds object code; a shared build maps a .so at load time. PIE/PIC allows relocation and sharing of read-only code pages. Missing library: `dlopen` returns NULL and `dlerror()` explains the failure.

**Expected principle:** static linking simplifies deployment but duplicates library code in each executable and requires relinking to update it. Dynamic linking reduces executable size and permits library updates, but introduces runtime dependency/version risk. `dlopen` delays mapping until the feature is required, trading lower idle memory/start-up work for explicit error handling.

Actual timings and RSS/PSS must be measured on the student's WSL machine; do not copy fabricated measurements.