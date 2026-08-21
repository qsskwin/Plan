from pathlib import Path

import matplotlib.pyplot as plt

from free_fall_euler import analytical_free_fall, simulate_free_fall


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

    final_position_errors = []

    for dt in DT_VALUES:
        times, states = simulate_free_fall(dt)
        numerical_positions = states[:, 0]
        numerical_velocities = states[:, 1]
        analytical_positions, _ = analytical_free_fall(times)
        position_errors = numerical_positions - analytical_positions

        label = f"Euler, dt={dt:g} s"
        position_axis.plot(times, numerical_positions, label=label)
        velocity_axis.plot(times, numerical_velocities, label=label)
        error_axis.plot(times, position_errors, label=f"dt={dt:g} s")
        final_position_errors.append(abs(position_errors[-1]))

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
        title="Euler Position Error",
        xlabel="Time (s)",
        ylabel="Numerical - analytical (m)",
    )

    step_axis.loglog(
        DT_VALUES,
        final_position_errors,
        "o-",
        label="Euler final error",
    )
    step_axis.set(
        title="Step Size vs Final Position Error",
        xlabel="Step size dt (s)",
        ylabel="Absolute final error (m)",
    )

    for axis in axes.flat:
        axis.grid(True, alpha=0.3)
        axis.legend()

    figure.suptitle("Explicit Euler Integration of 1-D NED Free Fall")
    figure.tight_layout()

    output_path = Path(__file__).with_name("free_fall_euler_comparison.png")
    figure.savefig(output_path, dpi=180, bbox_inches="tight")
    plt.close(figure)

    print(f"Saved figure: {output_path}")
    for dt, error in zip(DT_VALUES, final_position_errors):
        print(f"dt={dt:g} s, final absolute position error={error:.9f} m")


if __name__ == "__main__":
    main()
