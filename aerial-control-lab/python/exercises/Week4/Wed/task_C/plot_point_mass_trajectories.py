"""Plot the three Week 4 point-mass trajectory validation cases.

This plotting and presentation code is Codex-generated (G). Numerical
acceptance remains in the trajectory comparisons and is not inferred from the
appearance of the figures.
"""

from pathlib import Path

import matplotlib
import numpy as np

matplotlib.use("Agg")
import matplotlib.pyplot as plt

from aerial_control.point_mass import STANDARD_GRAVITY_MPS2, point_mass_derivative
from aerial_control.simulation import simulate_fixed_step


POSITION_ATOL_M = 1e-10
VELOCITY_ATOL_MPS = 1e-10
ERROR_FLOOR = 1e-18
OUTPUT_DIR = Path(__file__).resolve().parent / "outputs"

COMPONENTS = (
    ("N", "#2563EB", "o"),
    ("E", "#D97706", "s"),
    ("D", "#5B7F24", "^"),
)


def build_cases():
    """Return the three fixed Week 4 validation cases."""
    mass_kg = 1.5
    dt = 0.01
    identity_q_nb = np.array([1.0, 0.0, 0.0, 0.0])
    phi = np.deg2rad(10.0)
    roll_q_nb = np.array(
        [np.cos(phi / 2.0), np.sin(phi / 2.0), 0.0, 0.0]
    )

    return (
        {
            "slug": "free_fall",
            "title": "Free Fall",
            "mass_kg": mass_kg,
            "total_thrust_n": 0.0,
            "q_nb": identity_q_nb,
            "t0": 0.0,
            "tf": 2.0,
            "dt": dt,
            "analytical_acceleration_ned": np.array(
                [0.0, 0.0, STANDARD_GRAVITY_MPS2]
            ),
            "subtitle": (
                "m=1.5 kg | T=0 N | q_NB=[1, 0, 0, 0] | "
                "dt=0.01 s | t=0...2 s"
            ),
        },
        {
            "slug": "hover",
            "title": "Level Hover",
            "mass_kg": mass_kg,
            "total_thrust_n": mass_kg * STANDARD_GRAVITY_MPS2,
            "q_nb": identity_q_nb,
            "t0": 0.0,
            "tf": 10.0,
            "dt": dt,
            "analytical_acceleration_ned": np.zeros(3),
            "subtitle": (
                "m=1.5 kg | T=mg=14.709975 N | q_NB=[1, 0, 0, 0] | "
                "dt=0.01 s | t=0...10 s"
            ),
        },
        {
            "slug": "positive_roll",
            "title": "Fixed Positive Roll",
            "mass_kg": mass_kg,
            "total_thrust_n": (
                mass_kg * STANDARD_GRAVITY_MPS2 / np.cos(phi)
            ),
            "q_nb": roll_q_nb,
            "t0": 0.0,
            "tf": 2.0,
            "dt": dt,
            "analytical_acceleration_ned": np.array(
                [0.0, STANDARD_GRAVITY_MPS2 * np.tan(phi), 0.0]
            ),
            "subtitle": (
                "m=1.5 kg | roll=+10 deg | T=mg/cos(10 deg) | "
                "q_NB=[cos(5 deg), sin(5 deg), 0, 0] | dt=0.01 s | t=0...2 s"
            ),
        },
    )


def simulate_and_validate(case):
    """Simulate one case and return numerical, analytical, and error arrays."""
    initial_state = np.zeros(6)
    initial_state_before = initial_state.copy()

    def dynamics(t, state):
        return point_mass_derivative(
            t,
            state,
            mass_kg=case["mass_kg"],
            total_thrust_n=case["total_thrust_n"],
            q_nb=case["q_nb"],
        )

    times, states = simulate_fixed_step(
        dynamics,
        initial_state,
        case["t0"],
        case["tf"],
        case["dt"],
    )

    expected_updates = int(round((case["tf"] - case["t0"]) / case["dt"]))
    assert times.shape == (expected_updates + 1,)
    assert states.shape == (expected_updates + 1, 6)
    assert times[0] == case["t0"]
    assert times[-1] == case["tf"]
    assert np.all(np.isfinite(times))
    assert np.all(np.isfinite(states))
    np.testing.assert_array_equal(initial_state, initial_state_before)

    numerical_position = states[:, :3]
    numerical_velocity = states[:, 3:]
    acceleration = case["analytical_acceleration_ned"]
    elapsed = times - case["t0"]
    analytical_position = 0.5 * elapsed[:, None] ** 2 * acceleration[None, :]
    analytical_velocity = elapsed[:, None] * acceleration[None, :]

    position_error = np.abs(numerical_position - analytical_position)
    velocity_error = np.abs(numerical_velocity - analytical_velocity)

    np.testing.assert_allclose(
        numerical_position,
        analytical_position,
        atol=POSITION_ATOL_M,
        rtol=0.0,
    )
    np.testing.assert_allclose(
        numerical_velocity,
        analytical_velocity,
        atol=VELOCITY_ATOL_MPS,
        rtol=0.0,
    )

    return {
        "times": times,
        "states": states,
        "numerical_position": numerical_position,
        "numerical_velocity": numerical_velocity,
        "analytical_position": analytical_position,
        "analytical_velocity": analytical_velocity,
        "position_error": position_error,
        "velocity_error": velocity_error,
        "max_position_error_m": float(np.max(position_error)),
        "max_velocity_error_mps": float(np.max(velocity_error)),
    }


def style_time_axis(axis, *, ylabel):
    axis.set_xlabel("Time [s]")
    axis.set_ylabel(ylabel)
    axis.grid(True, color="#D1D5DB", linewidth=0.7, alpha=0.75)
    axis.spines["top"].set_visible(False)
    axis.spines["right"].set_visible(False)
    axis.spines["left"].set_color("#6B7280")
    axis.spines["bottom"].set_color("#6B7280")
    axis.tick_params(colors="#374151")


def plot_trajectory_panel(axis, times, numerical, analytical, *, title, ylabel):
    marker_stride = max(1, times.size // 20)
    for index, (label, color, marker) in enumerate(COMPONENTS):
        marker_offset = min(index * max(1, marker_stride // 3), times.size - 1)
        axis.plot(
            times,
            numerical[:, index],
            color=color,
            linewidth=1.8,
            marker=marker,
            markersize=4.0,
            markerfacecolor="white",
            markeredgewidth=1.0,
            markevery=(marker_offset, marker_stride),
            label=f"{label} numerical",
        )
        axis.plot(
            times,
            analytical[:, index],
            color=color,
            linewidth=1.4,
            linestyle="--",
            alpha=0.9,
            label=f"{label} analytical",
        )

    axis.axhline(0.0, color="#6B7280", linewidth=0.8, alpha=0.65)
    axis.set_title(title, color="#111827", fontsize=12, fontweight="semibold")
    style_time_axis(axis, ylabel=ylabel)
    axis.legend(ncol=2, fontsize=8, frameon=False, loc="best")


def plot_error_panel(axis, times, error, *, title, ylabel, tolerance):
    marker_stride = max(1, times.size // 20)
    for index, (label, color, marker) in enumerate(COMPONENTS):
        marker_offset = min(index * max(1, marker_stride // 3), times.size - 1)
        displayed_error = np.maximum(error[:, index], ERROR_FLOOR)
        axis.semilogy(
            times,
            displayed_error,
            color=color,
            linewidth=1.6,
            marker=marker,
            markersize=3.8,
            markerfacecolor="white",
            markeredgewidth=1.0,
            markevery=(marker_offset, marker_stride),
            label=label,
        )

    axis.axhline(
        tolerance,
        color="#111827",
        linewidth=1.2,
        linestyle=":",
        label=f"atol={tolerance:.0e}",
    )
    axis.set_ylim(ERROR_FLOOR / 2.0, tolerance * 10.0)
    axis.set_title(title, color="#111827", fontsize=12, fontweight="semibold")
    style_time_axis(axis, ylabel=ylabel)
    axis.legend(ncol=4, fontsize=8, frameon=False, loc="upper right")


def render_case(case, result):
    """Render one validated case as a four-panel PNG."""
    plt.rcParams.update(
        {
            "font.family": "DejaVu Sans",
            "font.size": 10,
            "axes.labelcolor": "#111827",
            "text.color": "#111827",
            "figure.facecolor": "#FFFFFF",
            "axes.facecolor": "#FFFFFF",
            "savefig.facecolor": "#FFFFFF",
        }
    )

    figure, axes = plt.subplots(2, 2, figsize=(14, 9.5))
    figure.subplots_adjust(
        left=0.075,
        right=0.98,
        bottom=0.12,
        top=0.83,
        hspace=0.34,
        wspace=0.24,
    )
    figure.suptitle(
        f"{case['title']} NED Point-Mass Trajectory",
        fontsize=17,
        fontweight="bold",
        color="#111827",
        y=0.965,
    )
    figure.text(
        0.5,
        0.92,
        case["subtitle"],
        ha="center",
        va="center",
        fontsize=10,
        color="#4B5563",
    )

    plot_trajectory_panel(
        axes[0, 0],
        result["times"],
        result["numerical_position"],
        result["analytical_position"],
        title="N/E/D position: numerical and analytical",
        ylabel="Position [m]",
    )
    plot_trajectory_panel(
        axes[0, 1],
        result["times"],
        result["numerical_velocity"],
        result["analytical_velocity"],
        title="N/E/D velocity: numerical and analytical",
        ylabel="Velocity [m/s]",
    )
    plot_error_panel(
        axes[1, 0],
        result["times"],
        result["position_error"],
        title="Absolute position error by NED component",
        ylabel="Absolute position error [m]",
        tolerance=POSITION_ATOL_M,
    )
    plot_error_panel(
        axes[1, 1],
        result["times"],
        result["velocity_error"],
        title="Absolute velocity error by NED component",
        ylabel="Absolute velocity error [m/s]",
        tolerance=VELOCITY_ATOL_MPS,
    )

    figure.text(
        0.5,
        0.045,
        (
            f"Max position error: {result['max_position_error_m']:.3e} m    |    "
            f"Max velocity error: {result['max_velocity_error_mps']:.3e} m/s    |    "
            f"Error display floor: {ERROR_FLOOR:.0e}"
        ),
        ha="center",
        fontsize=10,
        color="#374151",
    )

    OUTPUT_DIR.mkdir(parents=True, exist_ok=True)
    output_path = OUTPUT_DIR / f"{case['slug']}_trajectory.png"
    figure.savefig(output_path, dpi=200, bbox_inches="tight")
    plt.close(figure)
    return output_path


def main():
    for case in build_cases():
        result = simulate_and_validate(case)
        output_path = render_case(case, result)
        print(
            f"{case['title']}: "
            f"updates={result['times'].size - 1}, "
            f"states={result['states'].shape[0]}, "
            f"max_position_error={result['max_position_error_m']:.3e} m, "
            f"max_velocity_error={result['max_velocity_error_mps']:.3e} m/s"
        )
        print(f"  saved: {output_path}")


if __name__ == "__main__":
    main()
