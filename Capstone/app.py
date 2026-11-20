import platform
import shutil

import pandas as pd
import psutil
import streamlit as st

st.set_page_config(
    page_title="OSentinel",
    page_icon="🛡️",
    layout="wide"
)

st.title("🛡️ OSentinel")
st.subheader("Linux Resource Contention Detection & Analysis")
st.caption("Operating Systems Capstone Project | Version 2.0")

st.sidebar.title("OSentinel Menu")

page = st.sidebar.radio(
    "Select Module",
    [
        "System Dashboard",
        "Process Monitor",
        "System Information",
        "CPU Scheduling Lab",
        "Memory Management Lab",
        "Deadlock Management",
        "Disk Scheduling Lab",
        "Page Replacement Comparison",
        "Resource Contention Analyzer",
        "Recovery & Performance Report"
    ]
)


# ---------- SYSTEM DASHBOARD ----------
if page == "System Dashboard":
    st.header("System Resource Dashboard")

    cpu = psutil.cpu_percent(interval=1)
    memory = psutil.virtual_memory()
    disk = shutil.disk_usage("/")

    col1, col2, col3 = st.columns(3)
    col1.metric("CPU Usage", f"{cpu:.1f}%")
    col2.metric("RAM Usage", f"{memory.percent:.1f}%")
    col3.metric(
        "Disk Usage",
        f"{disk.used / disk.total * 100:.1f}%"
    )

    st.subheader("Resource Utilization")
    st.write("CPU")
    st.progress(int(cpu))
    st.write("Memory")
    st.progress(int(memory.percent))
    st.write("Disk")
    st.progress(int(disk.used / disk.total * 100))

    st.subheader("Resource Alerts")

    if cpu >= 80:
        st.warning("High CPU usage detected.")
    else:
        st.success("CPU usage is below the 80% warning threshold.")

    if memory.percent >= 80:
        st.warning("High memory usage detected.")
    else:
        st.success("Memory usage is below the 80% warning threshold.")

    st.caption("Threshold alerts do not by themselves prove a bottleneck.")


# ---------- PROCESS MONITOR ----------
elif page == "Process Monitor":
    st.header("Running Process Monitor")

    if st.button("Refresh Process List"):
        st.rerun()

    process_data = []

    for proc in psutil.process_iter(
        ["pid", "name", "status", "memory_percent"]
    ):
        try:
            p = proc.info
            process_data.append({
                "PID": p["pid"],
                "Process Name": p["name"] or "Unknown",
                "Status": p["status"] or "Unknown",
                "Memory Usage (%)": round(
                    p["memory_percent"] or 0, 2
                )
            })
        except (psutil.NoSuchProcess, psutil.AccessDenied):
            continue

    if process_data:
        df = pd.DataFrame(process_data)
        st.dataframe(
            df.sort_values("Memory Usage (%)", ascending=False),
            use_container_width=True,
            hide_index=True
        )
        st.write(f"Total visible processes: {len(df)}")
    else:
        st.info("No process information available.")


# ---------- SYSTEM INFORMATION ----------
elif page == "System Information":
    st.header("Operating System Information")

    st.write("**Operating System:**", platform.system())
    st.write("**OS Version:**", platform.release())
    st.write("**Machine Architecture:**", platform.machine())
    st.write("**Logical CPU Cores:**", psutil.cpu_count(logical=True))
    st.write("**Physical CPU Cores:**", psutil.cpu_count(logical=False))

    memory = psutil.virtual_memory()
    st.write(
        f"**Total RAM visible to Linux:** "
        f"{memory.total / (1024 ** 3):.2f} GB"
    )

    disk = shutil.disk_usage("/")
    st.write(
        f"**Filesystem Capacity:** "
        f"{disk.total / (1024 ** 3):.2f} GB"
    )

    st.info(
        "Under WSL2, these values may describe the Linux environment "
        "rather than the entire Windows system."
    )


# ---------- CPU SCHEDULING LAB ----------
elif page == "CPU Scheduling Lab":
    st.header("CPU Scheduling Simulator")
    st.write(
        "Compare scheduling algorithms using arrival times, "
        "burst times, and process priorities."
    )

    st.info(
        "This module simulates scheduling. It does not change "
        "the actual Linux CPU scheduler."
    )

    default_data = pd.DataFrame({
        "Process": ["P1", "P2", "P3", "P4"],
        "Arrival Time": [0, 1, 2, 3],
        "Burst Time": [5, 3, 8, 2],
        "Priority": [2, 1, 4, 3]
    })

    st.subheader("1. Enter Process Details")
    edited_data = st.data_editor(
        default_data,
        num_rows="dynamic",
        hide_index=True,
        use_container_width=True,
        key="process_editor"
    )

    algorithm = st.selectbox(
        "Choose Scheduling Algorithm",
        [
            "FCFS",
            "SJF (Non-preemptive)",
            "Priority (Non-preemptive)",
            "Round Robin"
        ]
    )

    quantum = 2
    if algorithm == "Round Robin":
        quantum = st.number_input(
            "Time Quantum",
            min_value=1,
            max_value=100,
            value=2,
            step=1
        )

    if st.button("Run Scheduling Simulation", type="primary"):
        try:
            data = edited_data.copy()
            data["Process"] = data["Process"].astype(str).str.strip()

            for col in ["Arrival Time", "Burst Time", "Priority"]:
                data[col] = pd.to_numeric(data[col], errors="raise")

            if data.empty:
                raise ValueError("Add at least one process.")

            if (
                data["Process"].eq("").any()
                or data["Process"].duplicated().any()
            ):
                raise ValueError(
                    "Process names must be non-empty and unique."
                )

            if (
                data[["Arrival Time", "Burst Time", "Priority"]]
                .isna().any().any()
            ):
                raise ValueError("Process values cannot be empty.")

            if (
                (data["Arrival Time"] < 0).any()
                or (data["Burst Time"] <= 0).any()
                or (data["Priority"] < 1).any()
            ):
                raise ValueError(
                    "Arrival times must be non-negative, burst times "
                    "positive, and priorities at least 1."
                )

            if not all(
                float(value).is_integer()
                for col in ["Arrival Time", "Burst Time", "Priority"]
                for value in data[col]
            ):
                raise ValueError(
                    "Arrival times, burst times, and priorities "
                    "must be whole numbers."
                )

            records = data.to_dict("records")
            remaining = {
                p["Process"]: int(p["Burst Time"]) for p in records
            }
            arrival = {
                p["Process"]: int(p["Arrival Time"]) for p in records
            }
            burst = remaining.copy()
            priority = {
                p["Process"]: int(p["Priority"]) for p in records
            }
            order = {p["Process"]: i for i, p in enumerate(records)}
            completion = {}
            timeline = []
            time_now = 0

            def execute(name, start, duration):
                end = start + duration
                timeline.append({
                    "Process": name,
                    "Start": start,
                    "End": end
                })
                return end

            if algorithm == "FCFS":
                pending = sorted(
                    records,
                    key=lambda p: (arrival[p["Process"]], order[p["Process"]])
                )
                for p in pending:
                    name = p["Process"]
                    time_now = max(time_now, arrival[name])
                    time_now = execute(name, time_now, burst[name])
                    completion[name] = time_now

            elif algorithm in [
                "SJF (Non-preemptive)",
                "Priority (Non-preemptive)"
            ]:
                completed = set()

                while len(completed) < len(records):
                    ready = [
                        p for p in records
                        if p["Process"] not in completed
                        and arrival[p["Process"]] <= time_now
                    ]

                    if not ready:
                        time_now = min(
                            arrival[p["Process"]]
                            for p in records
                            if p["Process"] not in completed
                        )
                        continue

                    if algorithm.startswith("SJF"):
                        chosen = min(
                            ready,
                            key=lambda p: (
                                burst[p["Process"]],
                                arrival[p["Process"]],
                                order[p["Process"]]
                            )
                        )
                    else:
                        chosen = min(
                            ready,
                            key=lambda p: (
                                priority[p["Process"]],
                                arrival[p["Process"]],
                                order[p["Process"]]
                            )
                        )

                    name = chosen["Process"]
                    time_now = execute(name, time_now, burst[name])
                    completion[name] = time_now
                    completed.add(name)

            else:  # Round Robin
                from collections import deque

                pending = sorted(
                    records,
                    key=lambda p: (arrival[p["Process"]], order[p["Process"]])
                )
                ready_queue = deque()
                index = 0

                while index < len(pending) or ready_queue:
                    if not ready_queue and index < len(pending):
                        time_now = max(
                            time_now, arrival[pending[index]["Process"]]
                        )

                    while (
                        index < len(pending)
                        and arrival[pending[index]["Process"]] <= time_now
                    ):
                        ready_queue.append(pending[index]["Process"])
                        index += 1

                    if not ready_queue:
                        continue

                    name = ready_queue.popleft()
                    duration = min(quantum, remaining[name])
                    time_now = execute(name, time_now, duration)
                    remaining[name] -= duration

                    while (
                        index < len(pending)
                        and arrival[pending[index]["Process"]] <= time_now
                    ):
                        ready_queue.append(pending[index]["Process"])
                        index += 1

                    if remaining[name] > 0:
                        ready_queue.append(name)
                    else:
                        completion[name] = time_now

            results = []
            for p in records:
                name = p["Process"]
                turnaround = completion[name] - arrival[name]
                waiting = turnaround - burst[name]
                results.append({
                    "Process": name,
                    "Arrival Time": arrival[name],
                    "Burst Time": burst[name],
                    "Priority": priority[name],
                    "Completion Time": completion[name],
                    "Turnaround Time": turnaround,
                    "Waiting Time": waiting
                })

            result_df = pd.DataFrame(results)

            st.subheader("2. Scheduling Results")
            st.dataframe(result_df, use_container_width=True, hide_index=True)

            col1, col2 = st.columns(2)
            col1.metric(
                "Average Waiting Time",
                f"{result_df['Waiting Time'].mean():.2f}"
            )
            col2.metric(
                "Average Turnaround Time",
                f"{result_df['Turnaround Time'].mean():.2f}"
            )

            st.subheader("3. Gantt Chart")

            chart_data = pd.DataFrame(timeline)
            chart_data["Duration"] = chart_data["End"] - chart_data["Start"]

            st.caption(
                "Each bar shows when a process uses the CPU. "
                "Gaps between bars represent idle CPU time."
            )

            st.vega_lite_chart(
                chart_data,
                {
                    "mark": {"type": "bar"},
                    "encoding": {
                        "x": {
                            "field": "Start",
                            "type": "quantitative",
                            "title": "Time",
                            "scale": {"zero": True}
                        },
                        "x2": {"field": "End"},
                        "y": {
                            "field": "Process",
                            "type": "nominal",
                            "title": "Process",
                            "sort": list(dict.fromkeys(
                                item["Process"] for item in timeline
                            ))
                        },
                        "tooltip": [
                            {"field": "Process", "type": "nominal"},
                            {"field": "Start", "type": "quantitative"},
                            {"field": "End", "type": "quantitative"},
                            {"field": "Duration", "type": "quantitative"}
                        ]
                    }
                },
                use_container_width=True
            )

            st.download_button(
                "Download Results as CSV",
                result_df.to_csv(index=False).encode("utf-8"),
                file_name="osentinel_scheduling_results.csv",
                mime="text/csv"
            )

        except (ValueError, TypeError) as error:
            st.error(f"Cannot run simulation: {error}")



# ---------- MEMORY MANAGEMENT LAB ----------
elif page == "Memory Management Lab":
    st.header("Memory Management Lab")
    st.write(
        "Explore contiguous memory allocation and virtual-memory "
        "page replacement algorithms."
    )

    st.subheader("1. Contiguous Memory Allocation")

    blocks_text = st.text_input(
        "Memory block sizes (comma-separated, units)",
        "100, 500, 200, 300, 600",
        key="memory_blocks"
    )
    processes_text = st.text_input(
        "Process memory requirements (comma-separated, units)",
        "212, 417, 112, 426",
        key="memory_processes"
    )
    allocation_method = st.selectbox(
        "Allocation strategy",
        ["First Fit", "Best Fit", "Worst Fit"]
    )

    if st.button("Run Memory Allocation"):
        try:
            blocks = [int(x.strip()) for x in blocks_text.split(",")]
            processes = [int(x.strip()) for x in processes_text.split(",")]

            if not blocks or not processes:
                raise ValueError("Enter at least one block and process.")
            if any(x <= 0 for x in blocks + processes):
                raise ValueError("All sizes must be positive integers.")

            remaining = blocks.copy()
            rows = []

            for i, size in enumerate(processes, start=1):
                candidates = [
                    j for j, available in enumerate(remaining)
                    if available >= size
                ]

                if not candidates:
                    rows.append({
                        "Process": f"P{i}",
                        "Requested Size": size,
                        "Allocated Block": "Not allocated",
                        "Unused Space": "-"
                    })
                    continue

                if allocation_method == "First Fit":
                    chosen = candidates[0]
                elif allocation_method == "Best Fit":
                    chosen = min(candidates, key=lambda j: remaining[j])
                else:
                    chosen = max(candidates, key=lambda j: remaining[j])

                before = remaining[chosen]
                remaining[chosen] -= size

                rows.append({
                    "Process": f"P{i}",
                    "Requested Size": size,
                    "Allocated Block": f"B{chosen + 1}",
                    "Unused Space": remaining[chosen]
                })

            st.dataframe(
                pd.DataFrame(rows),
                use_container_width=True,
                hide_index=True
            )
            st.caption(
                "This model splits each block's remaining free space. "
                "It is a simplified allocation simulation."
            )

        except ValueError as error:
            st.error(f"Invalid input: {error}")

    st.divider()
    st.subheader("2. Virtual Memory — Page Replacement")

    reference_text = st.text_input(
        "Page reference string (comma-separated)",
        "7, 0, 1, 2, 0, 3, 0, 4, 2, 3, 0, 3, 2",
        key="page_references"
    )
    frame_count = st.number_input(
        "Number of page frames",
        min_value=1,
        max_value=20,
        value=3,
        step=1
    )
    page_algorithm = st.selectbox(
        "Page replacement algorithm",
        ["FIFO", "LRU"]
    )

    if st.button("Run Page Replacement"):
        try:
            references = [
                int(x.strip()) for x in reference_text.split(",")
            ]
            if not references:
                raise ValueError("Enter a page reference string.")
            if any(x < 0 for x in references):
                raise ValueError("Page numbers must be non-negative.")

            frames = []
            history = []
            faults = 0
            last_used = {}

            for step, page_number in enumerate(references):
                hit = page_number in frames

                if not hit:
                    faults += 1
                    if len(frames) < frame_count:
                        frames.append(page_number)
                    else:
                        if page_algorithm == "FIFO":
                            frames.pop(0)
                        else:
                            victim = min(frames, key=lambda p: last_used[p])
                            frames.remove(victim)
                        frames.append(page_number)

                last_used[page_number] = step

                history.append({
                    "Step": step + 1,
                    "Page Requested": page_number,
                    **{
                        f"Frame {i + 1}": (
                            frames[i] if i < len(frames) else "-"
                        )
                        for i in range(frame_count)
                    },
                    "Result": "Hit" if hit else "Page Fault"
                })

            st.dataframe(
                pd.DataFrame(history),
                use_container_width=True,
                hide_index=True
            )

            col1, col2, col3 = st.columns(3)
            col1.metric("Page Faults", faults)
            col2.metric("Page Hits", len(references) - faults)
            col3.metric(
                "Fault Rate",
                f"{faults / len(references) * 100:.1f}%"
            )

            st.caption(
                "FIFO replaces the oldest loaded page. LRU replaces "
                "the page that has not been used for the longest time."
            )

        except ValueError as error:
            st.error(f"Invalid input: {error}")


elif page == "Deadlock Management":
    st.header("Deadlock Management")
    st.write(
        "Use Banker's Algorithm to check whether the system is in a "
        "safe state. This is a simulation; it does not change real processes."
    )

    processes = ["P0", "P1", "P2", "P3", "P4"]
    resources = ["A", "B", "C"]

    st.subheader("1. Allocation Matrix")
    allocation_default = pd.DataFrame(
        [[0, 1, 0], [2, 0, 0], [3, 0, 2], [2, 1, 1], [0, 0, 2]],
        index=processes, columns=resources
    )
    allocation_df = st.data_editor(
        allocation_default,
        key="deadlock_allocation",
        num_rows="fixed",
        use_container_width=True
    )

    st.subheader("2. Maximum Matrix")
    maximum_default = pd.DataFrame(
        [[7, 5, 3], [3, 2, 2], [9, 0, 2], [2, 2, 2], [4, 3, 3]],
        index=processes, columns=resources
    )
    maximum_df = st.data_editor(
        maximum_default,
        key="deadlock_maximum",
        num_rows="fixed",
        use_container_width=True
    )

    st.subheader("3. Available Resources")
    c1, c2, c3 = st.columns(3)
    with c1:
        available_a = st.number_input("Available A", min_value=0, max_value=100, value=3, step=1)
    with c2:
        available_b = st.number_input("Available B", min_value=0, max_value=100, value=3, step=1)
    with c3:
        available_c = st.number_input("Available C", min_value=0, max_value=100, value=2, step=1)

    if st.button("Run Safety Algorithm", type="primary"):
        try:
            allocation = allocation_df.astype(int).to_numpy()
            maximum = maximum_df.astype(int).to_numpy()
            available = [int(available_a), int(available_b), int(available_c)]

            if (allocation < 0).any() or (maximum < 0).any():
                raise ValueError("Resource values cannot be negative.")

            if (maximum < allocation).any():
                raise ValueError("Maximum resources cannot be less than allocation.")

            need = maximum - allocation

            st.subheader("4. Need Matrix (Maximum − Allocation)")
            st.dataframe(
                pd.DataFrame(need, index=processes, columns=resources),
                use_container_width=True
            )

            work = available.copy()
            finished = [False] * len(processes)
            safe_sequence = []
            steps = []

            while len(safe_sequence) < len(processes):
                progress = False
                for i, process in enumerate(processes):
                    if not finished[i] and all(
                        need[i][j] <= work[j] for j in range(len(resources))
                    ):
                        before = work.copy()
                        work = [
                            work[j] + allocation[i][j]
                            for j in range(len(resources))
                        ]
                        finished[i] = True
                        safe_sequence.append(process)
                        steps.append({
                            "Process": process,
                            "Work Before": str(before),
                            "Allocation Released": str(allocation[i].tolist()),
                            "Work After": str(work)
                        })
                        progress = True
                if not progress:
                    break

            if all(finished):
                st.success("Safe state: all processes can finish.")
                st.markdown("**Safe sequence:** " + " → ".join(safe_sequence))
            else:
                blocked = [
                    processes[i] for i, done in enumerate(finished) if not done
                ]
                st.error("Unsafe state: no complete safe sequence was found.")
                st.write("Processes that could not finish:", ", ".join(blocked))

            if steps:
                st.subheader("5. Algorithm Steps")
                st.dataframe(pd.DataFrame(steps), use_container_width=True)

        except (ValueError, TypeError) as error:
            st.error(f"Invalid resource input: {error}")


elif page == "Disk Scheduling Lab":
    import pandas as pd

    st.header("Module 7: Disk Scheduling Lab")
    st.write("Compare disk-head movement for FCFS, SSTF, SCAN and C-SCAN.")

    requests_text = st.text_input(
        "Disk requests (comma-separated)", "98,183,37,122,14,124,65,67"
    )
    disk_size = st.number_input("Disk size (number of cylinders)", 10, 10000, 200)
    head = st.number_input("Initial head position", 0, int(disk_size) - 1, 53)
    direction = st.selectbox("SCAN direction", ["Right", "Left"])

    try:
        requests = [int(x.strip()) for x in requests_text.split(",") if x.strip()]
        if not requests:
            raise ValueError("Enter at least one disk request.")
        if any(x < 0 or x >= disk_size for x in requests):
            raise ValueError("Every request must be within the disk range.")
        if len(requests) > 200:
            raise ValueError("Use at most 200 requests.")

        def disk_result(order):
            path_points = [int(head)] + list(order)
            movement = sum(abs(path_points[i] - path_points[i-1])
                           for i in range(1, len(path_points)))
            return path_points, movement

        def scan_order(circular=False):
            left = sorted([x for x in requests if x < head])
            right = sorted([x for x in requests if x >= head])
            if direction == "Right":
                order = right[:]
                if circular:
                    if order and order[-1] != disk_size - 1:
                        order.append(int(disk_size) - 1)
                    order += left
                else:
                    if order and order[-1] != disk_size - 1:
                        order.append(int(disk_size) - 1)
                    order += list(reversed(left))
            else:
                order = list(reversed(left))
                if circular:
                    if order and order[-1] != 0:
                        order.append(0)
                    order += list(reversed(right))
                else:
                    if order and order[-1] != 0:
                        order.append(0)
                    order += right
            # Remove artificial end points from the request display only
            # when they are not actual requests; movement includes disk edges.
            return order

        algorithms = st.multiselect(
            "Algorithms to compare",
            ["FCFS", "SSTF", "SCAN", "C-SCAN"],
            default=["FCFS", "SSTF", "SCAN", "C-SCAN"]
        )

        results = []
        for algorithm in algorithms:
            if algorithm == "FCFS":
                order = requests[:]
            elif algorithm == "SSTF":
                remaining = requests[:]
                order = []
                current = int(head)
                while remaining:
                    nxt = min(remaining, key=lambda x: abs(x - current))
                    order.append(nxt)
                    remaining.remove(nxt)
                    current = nxt
            elif algorithm == "SCAN":
                order = scan_order(False)
            else:
                order = scan_order(True)

            points, movement = disk_result(order)
            results.append({
                "Algorithm": algorithm,
                "Total head movement": movement,
                "Service order": " → ".join(map(str, order))
            })

        if results:
            st.subheader("Results")
            st.dataframe(pd.DataFrame(results), use_container_width=True)
            st.caption(
                "SCAN and C-SCAN include disk-edge travel where applicable. "
                "Service order may include an edge cylinder."
            )
            st.subheader("Head movement path")
            for algorithm in algorithms:
                if algorithm == "FCFS":
                    order = requests[:]
                elif algorithm == "SSTF":
                    remaining = requests[:]
                    order = []
                    current = int(head)
                    while remaining:
                        nxt = min(remaining, key=lambda x: abs(x-current))
                        order.append(nxt)
                        remaining.remove(nxt)
                        current = nxt
                elif algorithm == "SCAN":
                    order = scan_order(False)
                else:
                    order = scan_order(True)
                points, _ = disk_result(order)
                st.write(algorithm)
                st.line_chart(pd.DataFrame({
                    "Cylinder": points,
                    "Step": range(len(points))
                }).set_index("Step"))
    except ValueError as e:
        st.error(str(e))


elif page == "Page Replacement Comparison":
    import pandas as pd

    st.header("Module 8: Page Replacement Comparison")
    st.write("Compare FIFO, LRU and Optimal page replacement.")

    ref_text = st.text_input(
        "Page reference string (comma-separated)", "7,0,1,2,0,3,0,4,2,3,0,3,2"
    )
    frame_count = st.number_input("Number of frames", 1, 10, 3)

    try:
        refs = [int(x.strip()) for x in ref_text.split(",") if x.strip()]
        if not refs:
            raise ValueError("Enter at least one page reference.")
        if len(refs) > 200:
            raise ValueError("Use at most 200 page references.")
        if any(x < 0 for x in refs):
            raise ValueError("Page numbers cannot be negative.")

        def simulate_policy(policy):
            frames = []
            history = []
            faults = 0
            last_used = {}
            for i, page in enumerate(refs):
                fault = page not in frames
                if fault:
                    faults += 1
                    if len(frames) < frame_count:
                        frames.append(page)
                    else:
                        if policy == "FIFO":
                            victim = 0
                        elif policy == "LRU":
                            victim = min(
                                range(len(frames)),
                                key=lambda j: last_used.get(frames[j], -1)
                            )
                        else:
                            future = refs[i+1:]
                            distances = []
                            for candidate in frames:
                                distances.append(
                                    future.index(candidate)
                                    if candidate in future else float("inf")
                                )
                            victim = distances.index(max(distances))
                        frames[victim] = page
                last_used[page] = i
                history.append({
                    "Reference": page,
                    "Frames": " | ".join(map(str, frames)),
                    "Fault": "Yes" if fault else "No"
                })
            return faults, history

        policies = ["FIFO", "LRU", "Optimal"]
        summary = []
        histories = {}
        for policy in policies:
            faults, history = simulate_policy(policy)
            histories[policy] = history
            summary.append({
                "Policy": policy,
                "Page faults": faults,
                "Hits": len(refs) - faults,
                "Fault rate (%)": round(faults / len(refs) * 100, 2)
            })

        st.subheader("Comparison")
        st.dataframe(pd.DataFrame(summary), use_container_width=True)
        st.bar_chart(pd.DataFrame(summary).set_index("Policy")["Page faults"])

        selected = st.selectbox("View step-by-step history", policies)
        st.dataframe(pd.DataFrame(histories[selected]), use_container_width=True)
        st.download_button(
            "Download comparison CSV",
            pd.DataFrame(summary).to_csv(index=False),
            file_name="page_replacement_comparison.csv",
            mime="text/csv"
        )
    except ValueError as e:
        st.error(str(e))


elif page == "Resource Contention Analyzer":
    import psutil
    import pandas as pd

    st.header("Module 9: Resource Contention Analyzer")
    st.write(
        "Analyses current system measurements for possible resource pressure. "
        "This is a heuristic, not proof of a bottleneck."
    )

    cpu_limit = st.slider("CPU warning threshold (%)", 10, 100, 80)
    ram_limit = st.slider("RAM warning threshold (%)", 10, 100, 80)
    disk_limit = st.slider("Disk-space warning threshold (%)", 10, 100, 85)

    if st.button("Analyze current system"):
        cpu = psutil.cpu_percent(interval=1)
        memory = psutil.virtual_memory()
        disk = psutil.disk_usage("/")
        processes = []
        for proc in psutil.process_iter(
            ["pid", "name", "memory_percent", "cpu_percent", "status"]
        ):
            try:
                processes.append(proc.info)
            except (psutil.NoSuchProcess, psutil.AccessDenied):
                pass

        st.subheader("Current measurements")
        c1, c2, c3 = st.columns(3)
        c1.metric("CPU usage", f"{cpu:.1f}%")
        c2.metric("RAM usage", f"{memory.percent:.1f}%")
        c3.metric("Disk used", f"{disk.percent:.1f}%")

        findings = []
        if cpu >= cpu_limit:
            findings.append("High CPU utilization: inspect active CPU-heavy processes.")
        if memory.percent >= ram_limit:
            findings.append("High memory utilization: check memory-heavy processes.")
        if disk.percent >= disk_limit:
            findings.append("Disk space is running low: inspect large files and logs.")
        if not findings:
            findings.append(
                "No configured threshold was exceeded during this sample."
            )

        st.subheader("Findings and suggestions")
        for finding in findings:
            st.warning(finding) if "High " in finding or "low" in finding else st.info(finding)

        st.subheader("Top processes by memory")
        df = pd.DataFrame(processes)
        if not df.empty:
            df = df.sort_values("memory_percent", ascending=False).head(10)
            st.dataframe(df, use_container_width=True)
        else:
            st.info("No process information was available.")

        st.caption(
            "Measurements are a snapshot. Short spikes, I/O latency and workload "
            "context can affect interpretation."
        )


elif page == "Recovery & Performance Report":
    import psutil
    import pandas as pd
    from datetime import datetime

    st.header("Module 10: Recovery & Performance Report")
    st.write(
        "Generate a report with current measurements and safe recommendations. "
        "This module does not kill processes or modify kernel settings."
    )

    if st.button("Generate report"):
        cpu = psutil.cpu_percent(interval=1)
        mem = psutil.virtual_memory()
        disk = psutil.disk_usage("/")
        timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")

        recommendations = []
        if cpu >= 80:
            recommendations.append(
                "CPU: inspect CPU-intensive processes and review workload scheduling."
            )
        else:
            recommendations.append(
                "CPU: no high-utilization warning at the time of this sample."
            )
        if mem.percent >= 80:
            recommendations.append(
                "Memory: inspect memory-heavy processes and check for memory leaks."
            )
        else:
            recommendations.append(
                "Memory: no high-memory warning at the time of this sample."
            )
        if disk.percent >= 85:
            recommendations.append(
                "Disk: inspect large files and logs; remove only files you recognize."
            )
        else:
            recommendations.append(
                "Disk: no disk-capacity warning at the time of this sample."
            )

        report = [
            "OSentinel - System Performance Report",
            f"Generated: {timestamp}",
            "",
            "CURRENT MEASUREMENTS",
            f"CPU utilization: {cpu:.1f}%",
            f"RAM utilization: {mem.percent:.1f}%",
            f"RAM total: {mem.total / (1024**3):.2f} GiB",
            f"RAM available: {mem.available / (1024**3):.2f} GiB",
            f"Disk utilization (/): {disk.percent:.1f}%",
            f"Disk free (/): {disk.free / (1024**3):.2f} GiB",
            "",
            "RECOMMENDATIONS",
            *[f"- {item}" for item in recommendations],
            "",
            "NOTE: This report is informational. Recommendations are not automatic actions.",
        ]
        report_text = "\n".join(report)

        st.subheader("System summary")
        c1, c2, c3 = st.columns(3)
        c1.metric("CPU", f"{cpu:.1f}%")
        c2.metric("RAM", f"{mem.percent:.1f}%")
        c3.metric("Disk", f"{disk.percent:.1f}%")
        st.write("### Recommendations")
        for item in recommendations:
            st.write("- " + item)
        st.download_button(
            "Download system report",
            data=report_text,
            file_name="osentinel_performance_report.txt",
            mime="text/plain"
        )


st.divider()
st.caption("OSentinel | Linux Monitoring and OS Algorithm Simulation Framework")
