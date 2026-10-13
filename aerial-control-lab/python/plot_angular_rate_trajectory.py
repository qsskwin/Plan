"""Plot the formal torque-free three-axis angular-rate trajectory.

This auxiliary plotting script is Codex-generated (G). It reads actual C++
output; it does not integrate the model or compute conservation metrics.
"""

import argparse
import csv
import io
import os
import subprocess
from pathlib import Path

import matplotlib

matplotlib.use("Agg")
import matplotlib.pyplot as plt
import numpy as np


PROJECT_DIR = Path(__file__).resolve().parents[1]
BUILD_PRESET = "windows-mingw-gcc-debug" if os.name == "nt" else "ubuntu-gcc-debug"
EXECUTABLE = "angular_rate_demo.exe" if os.name == "nt" else "angular_rate_demo"
BINARY = PROJECT_DIR / "build" / BUILD_PRESET / EXECUTABLE
DEFAULT_OUTPUT = PROJECT_DIR / "docs" / "angular_rate_trajectory.png"
CASE_NAME = "torque-free three-axis coupling"
FIELDS = (
    "trajectory_time_s",
    "angular_rate_x_radps",
    "angular_rate_y_radps",
    "angular_rate_z_radps",
)
SERIES = (
    (r"$\omega_x$", "#2867B2", "-"),
    (r"$\omega_y$", "#C77716", "--"),
    (r"$\omega_z$", "#7B853F", "-."),
)


def read_trajectory(input_path):
    if input_path is None:
        result = subprocess.run(
            [str(BINARY)], check=True, capture_output=True, text=True, encoding="utf-8"
        )
        output = result.stdout
    else:
        output = input_path.read_text(encoding="utf-8-sig")

    lines = output.splitlines()
    case_start = lines.index(f"case={CASE_NAME}")
    case_end = next(
        (i for i in range(case_start + 1, len(lines)) if lines[i].startswith("case=")),
        len(lines),
    )
    case_lines = lines[case_start:case_end]
    if "validation=PASS" not in case_lines:
        raise ValueError("the selected C++ case did not report validation=PASS")
    header_index = case_lines.index(",".join(FIELDS))
    rows = list(csv.DictReader(io.StringIO("\n".join(case_lines[header_index:]))))
    data = np.asarray([[float(row[field]) for field in FIELDS] for row in rows])

    if data.shape != (201, 4) or not np.isfinite(data).all():
        raise ValueError("expected 201 finite samples with time and three angular rates")
    if not np.allclose(data[:, 0], np.arange(201) * 0.01, rtol=0.0, atol=1e-12):
        raise ValueError("sample times must cover 0 to 2 s at intervals of 0.01 s")
    if not np.array_equal(data[0, 1:], np.asarray([1.0, 2.0, 3.0])):
        raise ValueError("the initial angular rate must be [1, 2, 3] rad/s")
    return data


def plot_trajectory(data, output_path):
    plt.rcParams.update(
        {
            "font.sans-serif": ["Microsoft YaHei", "DejaVu Sans"],
            "axes.unicode_minus": False,
            "font.size": 12,
            "axes.labelcolor": "#252B32",
            "text.color": "#252B32",
            "xtick.color": "#444B53",
            "ytick.color": "#444B53",
        }
    )
    fig, axes = plt.subplots(figsize=(11.5, 6.8), facecolor="white")
    fig.subplots_adjust(left=0.10, right=0.97, bottom=0.19, top=0.78)
    fig.text(0.10, 0.94, "无外力矩三轴角速度轨迹", fontsize=21, fontweight="bold")
    fig.text(
        0.10,
        0.885,
        r"$J=\mathrm{diag}(0.02,\ 0.03,\ 0.04)\ \mathrm{kg\cdot m^2}$"
        r"    |    $\omega_B(0)=[1,\ 2,\ 3]^{\mathsf{T}}\ \mathrm{rad/s}$",
        fontsize=13,
        color="#555D66",
    )

    for column, (label, color, line_style) in enumerate(SERIES, start=1):
        axes.plot(
            data[:, 0],
            data[:, column],
            label=label,
            color=color,
            linestyle=line_style,
            linewidth=2.5,
        )

    axes.set_xlim(0.0, 2.0)
    axes.set_xticks(np.arange(0.0, 2.01, 0.25))
    axes.set_xlabel("时间 (s)", fontsize=14, labelpad=10)
    axes.set_ylabel("机体系角速度分量 (rad/s)", fontsize=14, labelpad=12)
    axes.axhline(0.0, color="#8B929A", linewidth=1.0, zorder=0)
    axes.grid(True, color="#E0E4E8", linewidth=0.8)
    axes.set_axisbelow(True)
    axes.spines[["top", "right"]].set_visible(False)
    axes.spines[["left", "bottom"]].set_color("#858D96")
    axes.legend(
        loc="lower left",
        bbox_to_anchor=(0.0, 1.01),
        ncol=3,
        frameon=False,
        fontsize=14,
        handlelength=3.3,
        borderaxespad=0.0,
    )
    fig.text(
        0.10,
        0.045,
        "实际 C++ RK4 轨迹  |  步长 0.01 s  |  201 个样本  |  FRD 机体系",
        fontsize=11,
        color="#555D66",
    )
    output_path.parent.mkdir(parents=True, exist_ok=True)
    fig.savefig(output_path, dpi=180, bbox_inches="tight", facecolor="white")
    plt.close(fig)
    return output_path


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--input", type=Path, help="saved angular_rate_demo stdout")
    parser.add_argument("--output", type=Path, default=DEFAULT_OUTPUT)
    args = parser.parse_args()
    data = read_trajectory(args.input)
    print(plot_trajectory(data, args.output))
    print(f"samples={len(data)}, time_s=[{data[0, 0]:g}, {data[-1, 0]:g}]")
    print("final_angular_rate_body_radps=" + ", ".join(f"{x:.17g}" for x in data[-1, 1:]))


if __name__ == "__main__":
    main()
