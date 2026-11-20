# Page-replacement experiment notes

Source: `page_replacement_experiment.c`  
Reference count: 100,000 per workload; frame counts 3–10; fixed seed 192565013. The locality workload selects from a small 8-page working set 80% of the time and otherwise from a 100-page range. The random workload selects uniformly from 100 pages. CSV results are in `ci_experiment_output.csv`.

A sample runner timing was approximately 19.357 ms for one 100,000-reference run of all three policies at 8 frames on the environment where it was tested. Runtime is hardware/compiler dependent; rerun locally and report your own timing. Optimal is the benchmark lower bound. FIFO may show Belady's anomaly; LRU generally benefits from locality but can still have many faults for a working set larger than available frames.

Open the CSV in Excel to create a line chart: x-axis = frames, y-axis = faults, one series per algorithm and workload. Include the chart image in your report.
