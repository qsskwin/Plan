"""Plot body-axis directions from the C++ attitude demo trajectory.

This auxiliary plotting script is Codex-generated (G). Numerical acceptance
comes from the C++ trajectory metrics, not from the appearance of the figure.
"""

import csv
import io
import os
import subprocess
from pathlib import Path

import matplotlib

matplotlib.use("Agg")
import matplotlib.pyplot as plt


PROJECT_DIR = Path(__file__).resolve().parents[1]
BUILD_PRESET = "windows-mingw-gcc-debug" if os.name == "nt" else "ubuntu-gcc-debug"
EXECUTABLE = "attitude_kinematics_demo.exe" if os.name == "nt" else "attitude_kinematics_demo"
BINARY = PROJECT_DIR / "build" / BUILD_PRESET / EXECUTABLE
OUTPUT = PROJECT_DIR / "docs" / "attitude_kinematics_axes.png"
CASE_NAME = "initial yaw 90 then body roll 90"

BODY_AXES = (
    ("x", "+x body (forward)", "#d94841"),
    ("y", "+y body (right)", "#27804d"),
    ("z", "+z body (down)", "#2867b2"),
)
NED_COMPONENTS = (("N", "#7754a8"), ("E", "#d08a20"), ("D", "#178f92"))


def read_cpp_trajectory():
    result = subprocess.run(
        [str(BINARY), "--trajectory-csv"],
        check=True,
        capture_output=True,
        text=True,
        encoding="utf-8",
    )
    rows = [row for row in csv.DictReader(io.StringIO(result.stdout)) if row["case"] == CASE_NAME]
    if len(rows) != 201:
        raise ValueError(f"expected 201 samples for {CASE_NAME}, got {len(rows)}")
    return rows


def plot_axes(rows):
    times = [float(row["time_s"]) for row in rows]
    fig = plt.figure(figsize=(12, 7.5))
    grid = fig.add_gridspec(3, 2, width_ratios=(1.15, 1), wspace=0.27, hspace=0.35)
    plane = fig.add_subplot(grid[:, 0])

    # The body forward axis stays East, outside this North-Down projection.
    # The other two body axes sweep quarter-circle paths in this plane.
    for axis_key, axis_label, body_color in BODY_AXES[1:]:
        north = [float(row[f"{axis_key}_N"]) for row in rows]
        down = [float(row[f"{axis_key}_D"]) for row in rows]
        plane.plot(north, down, color=body_color, linewidth=3, label=axis_label)
        plane.scatter(north[0], down[0], color="white", edgecolor=body_color, s=80, zorder=4)
        plane.scatter(north[100], down[100], color=body_color, marker="^", s=75, zorder=4)
        plane.scatter(north[-1], down[-1], color=body_color, marker="s", s=75, zorder=4)

    plane.axhline(0, color="#777777", linewidth=0.9)
    plane.axvline(0, color="#777777", linewidth=0.9)
    plane.text(-1.18, -0.08, "right starts: -North", color="#27804d", fontsize=9)
    plane.text(-0.55, 1.11, "right ends / down starts: +Down", color="#444444", fontsize=9)
    plane.text(0.56, -0.08, "down ends: +North", color="#2867b2", fontsize=9)
    plane.text(-0.95, 0.50, "Right (+y)", color="#27804d", fontsize=10)
    plane.text(0.47, 0.50, "Down (+z)", color="#2867b2", fontsize=10)
    plane.set_xlim(-1.25, 1.25)
    plane.set_ylim(-0.38, 1.3)
    plane.invert_yaxis()  # Positive NED Down points downward on the page.
    plane.set_aspect("equal", adjustable="box")
    plane.set_xlabel("North component (+N to the right)")
    plane.set_ylabel("Down component (+D downward)")
    plane.grid(True, alpha=0.2)

    for row_index, (axis_key, axis_label, _) in enumerate(BODY_AXES):
        component_plot = fig.add_subplot(grid[row_index, 1])
        for component, color in NED_COMPONENTS:
            values = [float(row[f"{axis_key}_{component}"]) for row in rows]
            component_plot.plot(times, values, color=color, linewidth=1.8, label=component)
        component_plot.set_ylim(-1.1, 1.1)
        component_plot.set_xlim(0.0, 2.0)
        component_plot.set_title(axis_label, fontsize=10)
        component_plot.set_ylabel("NED component")
        component_plot.grid(True, alpha=0.25)
        if row_index == 0:
            component_plot.legend(loc="lower right", ncol=3, fontsize=8)
        if row_index == 2:
            component_plot.set_xlabel("Time (s)")
        else:
            component_plot.set_xticklabels([])

    fig.suptitle("Body-axis directions, not aircraft position | +90° yaw, then +90° body roll", fontsize=14)
    fig.text(0.08, 0.80, "North–Down view of axis tips", fontsize=11)
    fig.text(0.08, 0.775, "circle: start | triangle: 1 s | square: finish", fontsize=9)
    fig.text(0.08, 0.745, "Forward (+x) stays East; out of this plane.", color="#a12d29", fontsize=10)
    fig.text(
        0.5, 0.02,
        "Actual C++ RK4 trajectory | dt = 0.01 s | 201 samples | q_NB maps body FRD to NED",
        ha="center", fontsize=9, color="#555555",
    )
    OUTPUT.parent.mkdir(parents=True, exist_ok=True)
    fig.savefig(OUTPUT, dpi=180, bbox_inches="tight")
    plt.close(fig)
    return OUTPUT


if __name__ == "__main__":
    print(plot_axes(read_cpp_trajectory()))
