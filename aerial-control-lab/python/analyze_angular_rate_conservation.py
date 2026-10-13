"""Summarize and plot conservation drift from actual C++ angular-rate samples.

This auxiliary analysis is Codex-generated (G). It reads an existing trajectory,
without integrating the model. Conservation statistics do not replace the
separate initial-angular-acceleration check.
"""

import argparse
import json
import math
from pathlib import Path

import matplotlib

matplotlib.use("Agg")
import matplotlib.pyplot as plt
from matplotlib.ticker import ScalarFormatter
import numpy as np

from plot_angular_rate_trajectory import BINARY, CASE_NAME, PROJECT_DIR, read_trajectory


INERTIA_KG_M2 = np.asarray([0.02, 0.03, 0.04])
RELATIVE_DRIFT_LIMIT = 1e-6
DEFAULT_FIGURE = PROJECT_DIR / "docs" / "angular_rate_conservation.png"
DEFAULT_METRICS = PROJECT_DIR / "docs" / "angular_rate_conservation_metrics.json"


def summarize_conservation(data, source):
    times = data[:, 0]
    omega = data[:, 1:]
    energy = 0.5 * np.sum(INERTIA_KG_M2 * omega**2, axis=1)
    momentum_norm = np.linalg.norm(INERTIA_KG_M2 * omega, axis=1)
    if not np.isfinite(energy).all() or not np.isfinite(momentum_norm).all():
        raise ValueError("conservation quantities must be finite")
    if not math.isclose(float(energy[0]), 0.25, rel_tol=1e-14, abs_tol=0.0):
        raise ValueError("initial energy does not match the independent reference")
    if not math.isclose(
        float(momentum_norm[0]), math.sqrt(0.0184), rel_tol=1e-14, abs_tol=0.0
    ):
        raise ValueError("initial angular momentum norm does not match the reference")

    # Cross-check all samples using scalar sums with a different summation path.
    scalar_energy = np.asarray(
        [
            0.5 * math.fsum(float(j) * float(w)**2 for j, w in zip(INERTIA_KG_M2, row))
            for row in omega
        ]
    )
    scalar_momentum_norm = np.asarray(
        [
            math.sqrt(math.fsum((float(j) * float(w))**2 for j, w in zip(INERTIA_KG_M2, row)))
            for row in omega
        ]
    )
    if not np.allclose(energy, scalar_energy, rtol=1e-14, atol=0.0):
        raise ValueError("vectorized and scalar energy calculations disagree")
    if not np.allclose(momentum_norm, scalar_momentum_norm, rtol=1e-14, atol=0.0):
        raise ValueError("vectorized and scalar momentum calculations disagree")

    energy_drift = np.abs(energy - energy[0]) / energy[0]
    momentum_drift = np.abs(momentum_norm - momentum_norm[0]) / momentum_norm[0]

    def summarize_quantity(values, drift, unit):
        max_index = int(np.argmax(drift))
        return {
            "unit": unit,
            "initial": float(values[0]),
            "final": float(values[-1]),
            "max_absolute_relative_drift": float(drift[max_index]),
            "time_of_max_drift_s": float(times[max_index]),
            "passed": bool(drift[max_index] <= RELATIVE_DRIFT_LIMIT),
        }

    metrics = {
        "case": CASE_NAME,
        "source": str(source.resolve()),
        "principal_inertia_kg_m2": INERTIA_KG_M2.tolist(),
        "sample_count": len(data),
        "dt_s": 0.01,
        "final_time_s": float(times[-1]),
        "relative_drift_limit": RELATIVE_DRIFT_LIMIT,
        "energy": summarize_quantity(energy, energy_drift, "J"),
        "angular_momentum_norm": summarize_quantity(momentum_norm, momentum_drift, "kg*m^2/s"),
        "scalar_crosscheck": {
            "max_absolute_energy_difference_J": float(np.max(np.abs(energy - scalar_energy))),
            "max_absolute_momentum_norm_difference_kg_m2_per_s": float(
                np.max(np.abs(momentum_norm - scalar_momentum_norm))
            ),
        },
        "provenance": "G",
    }
    metrics["both_invariants_passed"] = (
        metrics["energy"]["passed"] and metrics["angular_momentum_norm"]["passed"]
    )
    return energy_drift, momentum_drift, metrics


def plot_conservation(times, energy_drift, momentum_drift, metrics, output_path):
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
    fig, axes = plt.subplots(2, 1, figsize=(11.5, 8.5), sharex=True, sharey=True)
    fig.subplots_adjust(left=0.12, right=0.97, bottom=0.16, top=0.79, hspace=0.45)
    fig.text(0.12, 0.94, "无外力矩三轴运动：守恒漂移", fontsize=21, fontweight="bold")
    fig.text(
        0.12,
        0.89,
        "实际 C++ RK4 轨迹  |  步长 0.01 s  |  201 个样本  |  0–2 s",
        fontsize=13,
        color="#555D66",
    )
    y_max = max(float(np.max(energy_drift)), float(np.max(momentum_drift))) * 1.18
    series = (
        (
            energy_drift,
            "energy",
            "动能 E（初值 0.25 J）",
            r"$|E(t)-E_0|/E_0$",
            "#2867B2",
            "-",
        ),
        (
            momentum_drift,
            "angular_momentum_norm",
            r"角动量模长 H（初值 0.13564660 $\mathrm{kg\cdot m^2/s}$）",
            r"$|H(t)-H_0|/H_0$",
            "#C77716",
            "--",
        ),
    )
    for axis, (drift, key, title, label, color, style) in zip(axes, series):
        axis.plot(times, drift, color=color, linestyle=style, linewidth=2.4)
        axis.set_title(title, loc="left", fontsize=13, pad=16)
        axis.text(
            1.0,
            1.065,
            f"最大相对漂移：{metrics[key]['max_absolute_relative_drift']:.4e}",
            transform=axis.transAxes,
            ha="right",
            fontsize=12,
            color="#555D66",
        )
        axis.set_xlim(0.0, 2.0)
        axis.set_ylim(0.0, max(y_max, 1e-16))
        axis.set_xticks(np.arange(0.0, 2.01, 0.25))
        axis.set_ylabel(label, fontsize=14, labelpad=12)
        formatter = ScalarFormatter(useMathText=True)
        formatter.set_useOffset(False)
        formatter.set_powerlimits((-3, 3))
        axis.yaxis.set_major_formatter(formatter)
        axis.grid(True, color="#E0E4E8", linewidth=0.8)
        axis.set_axisbelow(True)
        axis.spines[["top", "right"]].set_visible(False)
        axis.spines[["left", "bottom"]].set_color("#858D96")
    axes[-1].set_xlabel("时间 (s)", fontsize=14, labelpad=10)
    fig.text(
        0.12,
        0.045,
        r"最大相对漂移验收阈值：$10^{-6}$。两个面板共用纵轴尺度，显示实际漂移范围。",
        fontsize=11,
        color="#555D66",
    )
    output_path.parent.mkdir(parents=True, exist_ok=True)
    fig.savefig(output_path, dpi=180, bbox_inches="tight", facecolor="white")
    plt.close(fig)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--input", type=Path, help="saved angular_rate_demo stdout")
    parser.add_argument("--figure", type=Path, default=DEFAULT_FIGURE)
    parser.add_argument("--metrics", type=Path, default=DEFAULT_METRICS)
    args = parser.parse_args()
    data = read_trajectory(args.input)
    source = args.input if args.input is not None else BINARY
    energy_drift, momentum_drift, metrics = summarize_conservation(data, source)
    plot_conservation(data[:, 0], energy_drift, momentum_drift, metrics, args.figure)
    args.metrics.parent.mkdir(parents=True, exist_ok=True)
    args.metrics.write_text(json.dumps(metrics, indent=2) + "\n", encoding="utf-8")
    print(json.dumps(metrics, indent=2))
    print(f"figure={args.figure}")
    print(f"metrics={args.metrics}")
    return 0 if metrics["both_invariants_passed"] else 1


if __name__ == "__main__":
    raise SystemExit(main())
