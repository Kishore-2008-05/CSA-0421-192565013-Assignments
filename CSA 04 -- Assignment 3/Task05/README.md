# Q5 – Page replacement
Compile:
```bash
gcc -std=c11 -Wall -Wextra page_replacement.c -o page_replacement
gcc -std=c11 -Wall -Wextra page_replacement_experiment.c -o page_replacement_experiment
```
Run both. The first reports FIFO/LRU/Optimal for the assignment strings and Belady's anomaly. The experiment generates 100,000-reference locality and random workloads and reports fault counts for 3–10 frames. CSV output is in `ci_experiment_output.csv`; generated charts are `locality_page_faults_chart.svg` and `random_page_faults_chart.svg`. Timing depends on hardware/compiler; rerun locally. See `experiment_notes.md` for the seed and workload definition.
