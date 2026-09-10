from pathlib import Path

import numpy as np


POSITION_ATOL_M = 1e-10
VELOCITY_ATOL_MPS = 1e-10
GRAVITY_MPS2 = 9.80665
ROLL_RAD = np.deg2rad(10.0)

TASK_B_DIR = Path(__file__).resolve().parent
TRAJECTORY_DIR = TASK_B_DIR.parent / "task_A" / "outputs"
REPORT_PATH = TASK_B_DIR / "comparison_report.md"

NUMERICAL_COLUMNS = (
    "num_p_n_m",
    "num_p_e_m",
    "num_p_d_m",
    "num_v_n_mps",
    "num_v_e_mps",
    "num_v_d_mps",
)

CASE_SPECS = (
    {"slug": "free_fall", "display_name": "Free fall", "tf_s": 2.0},
    {"slug": "hover", "display_name": "Level hover", "tf_s": 10.0},
    {
        "slug": "positive_roll",
        "display_name": "Fixed positive roll",
        "tf_s": 2.0,
    },
)


def build_independent_analytical_states(slug, times):
    states = np.zeros((times.size, 6), dtype=float)
    if slug == "free_fall":
        states[:, 2] = 0.5 * GRAVITY_MPS2 * times ** 2
        states[:, 5] = GRAVITY_MPS2 * times
    elif slug == "hover":
        pass
    elif slug == "positive_roll":
        acceleration_east_mps2 = GRAVITY_MPS2 * np.tan(ROLL_RAD)
        states[:, 1] = 0.5 * acceleration_east_mps2 * times ** 2
        states[:, 4] = acceleration_east_mps2 * times
    elif slug == "mass_bias":
        actual_mass_kg = 1.65
        nominal_hover_thrust_n = 14.709975
        acceleration_down_mps2 = (
            GRAVITY_MPS2 - nominal_hover_thrust_n / actual_mass_kg
        )
        states[:, 2] = 0.5 * acceleration_down_mps2 * times ** 2
        states[:, 5] = acceleration_down_mps2 * times
    else:
        raise ValueError(f"Unknown case: {slug}")
    return states


def load_numerical_trajectory(language, slug):
    path = TRAJECTORY_DIR / f"{language}_{slug}.csv"
    table = np.genfromtxt(
        path,
        delimiter=",",
        names=True,
        dtype=float,
        encoding="utf-8",
    )
    times = np.asarray(table["time_s"], dtype=float)
    states = np.column_stack([table[name] for name in NUMERICAL_COLUMNS])
    return path, times, states


def split_max_errors(lhs, rhs):
    absolute_error = np.abs(lhs - rhs)
    return (
        float(np.max(absolute_error[:, :3])),
        float(np.max(absolute_error[:, 3:])),
    )


def compare_case(spec):
    python_path, python_times, python_states = load_numerical_trajectory(
        "python", spec["slug"]
    )
    cpp_path, cpp_times, cpp_states = load_numerical_trajectory(
        "cpp", spec["slug"]
    )

    expected_steps = int(round(spec["tf_s"] / 0.01))
    expected_times = np.arange(expected_steps + 1, dtype=float) * 0.01
    expected_times[-1] = spec["tf_s"]
    if python_times.shape != (expected_steps + 1,):
        raise AssertionError(f"Unexpected Python sample count for {spec['slug']}")
    if cpp_times.shape != (expected_steps + 1,):
        raise AssertionError(f"Unexpected C++ sample count for {spec['slug']}")
    np.testing.assert_array_equal(python_times, expected_times)
    np.testing.assert_array_equal(cpp_times, expected_times)
    np.testing.assert_array_equal(python_times, cpp_times)

    analytical_states = build_independent_analytical_states(
        spec["slug"], expected_times
    )
    np.testing.assert_allclose(
        python_states[:, :3],
        analytical_states[:, :3],
        rtol=0.0,
        atol=POSITION_ATOL_M,
    )
    np.testing.assert_allclose(
        python_states[:, 3:],
        analytical_states[:, 3:],
        rtol=0.0,
        atol=VELOCITY_ATOL_MPS,
    )
    np.testing.assert_allclose(
        cpp_states[:, :3],
        analytical_states[:, :3],
        rtol=0.0,
        atol=POSITION_ATOL_M,
    )
    np.testing.assert_allclose(
        cpp_states[:, 3:],
        analytical_states[:, 3:],
        rtol=0.0,
        atol=VELOCITY_ATOL_MPS,
    )
    np.testing.assert_allclose(
        python_states[:, :3],
        cpp_states[:, :3],
        rtol=0.0,
        atol=POSITION_ATOL_M,
    )
    np.testing.assert_allclose(
        python_states[:, 3:],
        cpp_states[:, 3:],
        rtol=0.0,
        atol=VELOCITY_ATOL_MPS,
    )

    return {
        "case": spec["display_name"],
        "samples": expected_steps + 1,
        "python_path": python_path,
        "cpp_path": cpp_path,
        "python_analytical": split_max_errors(
            python_states, analytical_states
        ),
        "cpp_analytical": split_max_errors(cpp_states, analytical_states),
        "python_cpp": split_max_errors(python_states, cpp_states),
    }


def format_value(value):
    return f"{value:.17g}"


def write_report(results, report_path=REPORT_PATH):
    lines = [
        "# Week 4 point-mass cross-language comparison",
        "",
        (
            "Every saved time and every state component was compared with "
            "`rtol=0`. Position uses `atol=1e-10 m`; velocity uses "
            "`atol=1e-10 m/s`. The analytical trajectories below are "
            "constructed independently by this comparison program."
        ),
        "",
        (
            "| Case | Samples | Python to analytical position (m) | "
            "Python to analytical velocity (m/s) | "
            "C++ to analytical position (m) | "
            "C++ to analytical velocity (m/s) | "
            "Python to C++ position (m) | Python to C++ velocity (m/s) |"
        ),
        "|---|---:|---:|---:|---:|---:|---:|---:|",
    ]
    for result in results:
        py_pos, py_vel = result["python_analytical"]
        cpp_pos, cpp_vel = result["cpp_analytical"]
        cross_pos, cross_vel = result["python_cpp"]
        lines.append(
            f"| {result['case']} | {result['samples']} | "
            f"{format_value(py_pos)} | {format_value(py_vel)} | "
            f"{format_value(cpp_pos)} | {format_value(cpp_vel)} | "
            f"{format_value(cross_pos)} | {format_value(cross_vel)} |"
        )
    lines.extend(
        [
            "",
            "All six comparisons for every case satisfy their absolute tolerance.",
            "",
        ]
    )
    report_path.parent.mkdir(parents=True, exist_ok=True)
    report_path.write_text("\n".join(lines), encoding="utf-8")


def main():
    results = [compare_case(spec) for spec in CASE_SPECS]
    write_report(results)
    print(REPORT_PATH.read_text(encoding="utf-8"))
    print(f"Report: {REPORT_PATH}")


if __name__ == "__main__":
    main()
