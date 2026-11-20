#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
OUT="$ROOT/local-evidence/$(date +%Y%m%d_%H%M%S)"
mkdir -p "$OUT"
run(){ local name="$1"; shift; echo "===== $name ====="; "$@" 2>&1 | tee "$OUT/$name.txt"; }
echo "Evidence directory: $OUT"
echo "Host: $(uname -a)" | tee "$OUT/environment.txt"
gcc --version | head -1 | tee -a "$OUT/environment.txt"
getconf PAGESIZE | tee -a "$OUT/environment.txt"

D3="$ROOT/CSA 04 -- Assignment 3"
cd "$D3/Task01"
gcc -std=c11 -Wall -Wextra -fPIC -shared crypto_plugin.c -o libcrypto_plugin.so
gcc -std=c11 -Wall -Wextra -c crypto_plugin.c -o crypto_plugin.o
gcc -std=c11 -Wall -Wextra link_static_demo.c crypto_plugin.o -o link_static_demo
gcc -std=c11 -Wall -Wextra link_dynamic_demo.c -L. -lcrypto_plugin -Wl,-rpath,'$ORIGIN' -o link_dynamic_demo
gcc -std=c11 -Wall -Wextra link_demo.c -o link_demo -ldl
run co3_q1_static ./link_static_demo
run co3_q1_dynamic ./link_dynamic_demo
run co3_q1_dlopen ./link_demo
run co3_q1_ldd ldd ./link_dynamic_demo
stat -c '%n %s bytes' link_static_demo link_dynamic_demo | tee "$OUT/co3_q1_sizes.txt"

cd "$D3/Task02"; gcc -std=c11 -Wall -Wextra paging_translation.c -o paging_translation; gcc -std=c11 -Wall -Wextra paging_fragmentation.c -o paging_fragmentation; run co3_q2 ./paging_translation; run co3_q2_fragmentation ./paging_fragmentation
cd "$D3/Task03"; gcc -std=c11 -Wall -Wextra segmentation.c -o segmentation; gcc -std=c11 -Wall -Wextra segmentation_fragmentation.c -o segmentation_fragmentation; run co3_q3 ./segmentation; run co3_q3_fragmentation ./segmentation_fragmentation
cd "$D3/Task04"; gcc -std=c11 -Wall -Wextra demand_paging.c -o demand_paging; run co3_q4 ./demand_paging 16
cd "$D3/Task05"; gcc -std=c11 -Wall -Wextra page_replacement.c -o page_replacement; gcc -std=c11 -Wall -Wextra page_replacement_experiment.c -o page_replacement_experiment; run co3_q5 ./page_replacement; ./page_replacement_experiment > "$OUT/co3_q5_experiment.csv" 2> "$OUT/co3_q5_timing.txt"
cd "$D3/Task06"; gcc -std=c11 -Wall -Wextra raid_capacity.c -o raid_capacity; run co3_q6 ./raid_capacity
cd "$D3/Task07"; gcc -std=c11 -Wall -Wextra mlock_latency.c -o mlock_latency; if ./mlock_latency 1 > "$OUT/co3_q7_mlock.txt" 2>&1; then cat "$OUT/co3_q7_mlock.txt"; else echo "mlock unavailable under current RLIMIT_MEMLOCK; see $OUT/co3_q7_mlock.txt"; fi

D4="$ROOT/CSA 04 --Assignment 4"
cd "$D4/Task02"; gcc -std=c11 -Wall -Wextra file_allocation.c -o file_allocation; gcc -std=c11 -Wall -Wextra inode_capacity.c -o inode_capacity; run co4_q2_allocation ./file_allocation; run co4_q2_inode ./inode_capacity
cd "$D4/Task04"; gcc -std=c11 -Wall -Wextra disk_scheduling.c -o disk_scheduling; gcc -std=c11 -Wall -Wextra disk_scheduling_all.c -o disk_scheduling_all; gcc -std=c11 -Wall -Wextra disk_queue_simulation.c -o disk_queue_simulation; run co4_q4_static ./disk_scheduling_all; run co4_q4_load ./disk_queue_simulation
cd "$D4/Task06"; gcc -std=c11 -Wall -Wextra storage_workload.c -o storage_workload; run co4_q6 ./storage_workload

D5="$ROOT/CSA 04 -- Assignment 5"
run co5_b1_log bash "$D5/TaskB1/log_summary.sh"
run co5_b1_disk_alert bash "$D5/TaskB1/disk_alert.sh" 80 /
echo "Portable test run complete. Outputs saved in: $OUT"
