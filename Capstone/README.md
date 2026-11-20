# OSentinel – Linux Resource Contention Detection & Analysis

**Operating Systems Capstone Project | Version 2.0**

OSentinel is a Streamlit-based application that combines live Linux system monitoring with interactive simulations of core Operating System concepts. It provides a single interface to inspect CPU, memory and disk usage, view running processes, experiment with scheduling and memory-management algorithms, analyze resource pressure, and generate a system performance report.

> **Scope note:** OS monitoring modules display measurements from the environment in which the app runs. Algorithm modules are educational simulations; they do not change the real Linux scheduler, allocate physical memory, or control operating-system processes. Resource alerts are threshold-based indicators and should not be treated as definitive proof of a bottleneck.

## Project files

- [Application source code – app.py](./app.py)
- [Project presentation – OSentinel OS.pptx](./OSentinel%20OS.pptx)

## Modules and implementation

### 1. System Dashboard
- Reads current CPU utilization using `psutil`.
- Displays RAM utilization and disk-space usage.
- Presents resource values as metrics and progress indicators.
- Flags CPU usage at or above 80%.
- Flags memory usage at or above 80%.

### 2. Process Monitor
- Retrieves visible process information from the operating system.
- Displays process ID, process name and process status.
- Shows each process's memory percentage.
- Sorts the process table by memory usage.
- Provides a refresh control to reload the process list.

### 3. System Information
- Displays operating-system name and release.
- Reports machine architecture.
- Shows logical and physical CPU core counts.
- Displays RAM visible to the Linux environment.
- Reports filesystem capacity and includes a note about WSL2 reporting.

### 4. CPU Scheduling Lab
- Accepts editable process names, arrival times, burst times and priorities.
- Simulates First Come First Served (FCFS).
- Simulates non-preemptive Shortest Job First (SJF).
- Simulates non-preemptive Priority scheduling.
- Simulates Round Robin with a configurable time quantum and displays a Gantt-style timeline, waiting time and turnaround time.

### 5. Memory Management Lab
- Accepts memory block sizes and process memory requirements.
- Simulates First Fit, Best Fit and Worst Fit allocation strategies.
- Displays allocated blocks and remaining space.
- Accepts a page-reference string and frame count.
- Simulates FIFO and LRU page replacement with page-hit and page-fault statistics.

### 6. Deadlock Management
- Provides editable Allocation and Maximum matrices.
- Accepts the Available resource vector for three resource types.
- Calculates the Need matrix using Maximum − Allocation.
- Applies the Banker's safety algorithm.
- Displays a safe sequence when all processes can finish, or reports processes that could not finish.

### 7. Disk Scheduling Lab
- Accepts disk requests, disk size, initial head position and scan direction.
- Simulates FCFS and SSTF disk scheduling.
- Simulates SCAN and C-SCAN disk scheduling.
- Compares total disk-head movement and service order.
- Displays the head movement path for the selected algorithms.

### 8. Page Replacement Comparison
- Accepts a page-reference string and number of frames.
- Compares FIFO, LRU and Optimal page replacement.
- Reports page faults, page hits and fault rate.
- Displays a bar chart of page faults.
- Provides step-by-step replacement history and CSV export.

### 9. Resource Contention Analyzer
- Lets the user configure CPU, RAM and disk-space warning thresholds.
- Samples current CPU, memory and disk utilization.
- Reports which configured thresholds have been exceeded.
- Lists the top processes by memory usage.
- Provides suggestions for investigating possible resource pressure.

### 10. Recovery & Performance Report
- Collects a timestamped CPU, RAM and disk usage snapshot.
- Generates informational recommendations based on utilization thresholds.
- Displays a system summary.
- Allows the report to be downloaded as a text file.

> The analyzer and report provide observations and recommendations only. They do not terminate processes, modify kernel settings, or perform automatic recovery actions.

## Technology stack

| Technology | Purpose |
|---|---|
| Python | Application programming language |
| Streamlit | Interactive web application interface |
| psutil | CPU, memory and process monitoring |
| pandas | Tabular data handling and result presentation |
| Python standard library | Platform details, disk usage, and supporting operations |

## Requirements

- Python 3.9 or later is recommended.
- Linux or Ubuntu on WSL is recommended for the Linux monitoring use case.
- Internet access may be needed to install the Python packages.

## Installation and execution

### 1. Clone the repository

```bash
git clone https://github.com/Kishore-2008-05/CSA-0421-192565013-Assignments.git
cd CSA-0421-192565013-Assignments/Capstone
```

### 2. Create and activate a virtual environment (recommended)

```bash
python3 -m venv .venv
source .venv/bin/activate
```

### 3. Install dependencies

```bash
python -m pip install --upgrade pip
pip install streamlit pandas psutil
```

### 4. Run OSentinel

```bash
streamlit run app.py
```

Streamlit will display a local URL in the terminal (normally `http://localhost:8501`). Open that address in a browser.

### Running in Ubuntu on WSL

Open the Ubuntu terminal, change to the project directory, activate the virtual environment if you created one, and run:

```bash
cd ~/osentinel
streamlit run app.py
```

If you are running the copy cloned from GitHub instead, use the directory where you cloned the repository.

## Using the application

1. Start the application with `streamlit run app.py`.
2. Use the **OSentinel Menu** in the sidebar to select a module.
3. For monitoring modules, review the current system measurements.
4. For simulation modules, enter the process, memory, page or disk inputs and run the simulation.
5. Review tables, charts, safe-sequence results and generated reports where available.

## Project presentation

The PowerPoint presentation is available here:

[**OSentinel OS – Capstone Presentation**](./OSentinel%20OS.pptx)

## Limitations

- Monitoring values depend on the permissions and environment where Streamlit is running.
- In WSL2, reported system information and resource values may describe the Linux environment rather than the complete Windows host.
- Resource contention warnings are heuristic threshold checks based on a snapshot.
- Scheduling, memory allocation, deadlock and disk/page replacement features are simulations for learning and analysis.
- The performance report is informational; it does not automatically change system configuration or recover resources.

## Project status

The repository contains the Streamlit application source and the capstone presentation. The README documents the functionality implemented in `app.py`.
