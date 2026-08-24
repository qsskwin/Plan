from pathlib import Path

import matplotlib.pyplot as plt
import numpy as np

from free_fall_euler import (
    analytical_free_fall,
    simulate_free_fall,
    simulate_free_fall_rk4,
)


DT_VALUES = (0.1, 0.05, 0.01)


def main():
    figure, axes = plt.subplots(2, 2, figsize=(12, 8))
    position_axis, velocity_axis = axes[0]
    error_axis, step_axis = axes[1]

    exact_times, _ = simulate_free_fall(min(DT_VALUES))
    exact_positions, exact_velocities = analytical_free_fall(exact_times)
    position_axis.plot(
        exact_times,
        exact_positions,
        "k--",
        linewidth=2,
        label="Analytical",
    )
    velocity_axis.plot(
        exact_times,
        exact_velocities,
        "k--",
        linewidth=2,
        label="Analytical",
    )

    final_euler_errors = []
    final_rk4_errors = []

    for dt in DT_VALUES:
        times, euler_states = simulate_free_fall(dt)
        rk4_times, rk4_states = simulate_free_fall_rk4(dt)
        np.testing.assert_allclose(rk4_times, times)

        analytical_positions, _ = analytical_free_fall(times)
        euler_position_errors = euler_states[:, 0] - analytical_positions
        rk4_position_errors = rk4_states[:, 0] - analytical_positions

        position_axis.plot(
            times, euler_states[:, 0], label=f"Euler, dt={dt:g} s"
        )
        position_axis.plot(
            times,
            rk4_states[:, 0],
            ":",
            linewidth=2,
            label=f"RK4, dt={dt:g} s",
        )
        velocity_axis.plot(
            times, euler_states[:, 1], label=f"Euler, dt={dt:g} s"
        )
        velocity_axis.plot(
            times,
            rk4_states[:, 1],
            ":",
            linewidth=2,
            label=f"RK4, dt={dt:g} s",
        )
        error_axis.plot(
            times,
            euler_position_errors,
            label=f"Euler, dt={dt:g} s",
        )
        error_axis.plot(
            times,
            rk4_position_errors,
            ":",
            linewidth=2,
            label=f"RK4, dt={dt:g} s",
        )
        final_euler_errors.append(abs(euler_position_errors[-1]))
        final_rk4_errors.append(abs(rk4_position_errors[-1]))

    position_axis.set(
        title="NED Free Fall: Position",
        xlabel="Time (s)",
        ylabel="Down position z (m)",
    )
    velocity_axis.set(
        title="NED Free Fall: Velocity",
        xlabel="Time (s)",
        ylabel="Down velocity v_z (m/s)",
    )
    error_axis.set(
        title="Position Error",
        xlabel="Time (s)",
        ylabel="Numerical - analytical (m)",
    )

    step_axis.loglog(
        DT_VALUES,
        final_euler_errors,
        "o-",
        label="Euler final error",
    )
    # Logarithmic axes cannot display exact zero. Preserve the real errors for
    # printed output and clamp only the plotted RK4 values to machine epsilon.
    rk4_errors_for_plot = np.maximum(final_rk4_errors, np.finfo(float).eps)
    step_axis.loglog(
        DT_VALUES,
        rk4_errors_for_plot,
        "s--",
        label="RK4 final error",
    )
    step_axis.set(
        title="Step Size vs Final Position Error",
        xlabel="Step size dt (s)",
        ylabel="Absolute final error (m)",
    )

    for axis in axes.flat:
        axis.grid(True, alpha=0.3)
        axis.legend()

    figure.suptitle("Euler and RK4 Integration of 1-D NED Free Fall")
    figure.tight_layout()

    output_path = Path(__file__).with_name("free_fall_integrator_comparison.png")
    figure.savefig(output_path, dpi=180, bbox_inches="tight")
    plt.close(figure)

    print(f"Saved figure: {output_path}")
    print("dt (s) | Euler final | RK4 final | Euler |z error| | RK4 |z error|")
    for dt, euler_error, rk4_error in zip(
        DT_VALUES,
        final_euler_errors,
        final_rk4_errors,
    ):
        _, euler_states = simulate_free_fall(dt)
        _, rk4_states = simulate_free_fall_rk4(dt)
        print(
            f"{dt:6g} | {euler_states[-1]} | {rk4_states[-1]} | "
            f"{euler_error:.9e} | {rk4_error:.9e}"
        )


if __name__ == "__main__":
    main()
