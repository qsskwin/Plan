import argparse
from pathlib import Path

import numpy as np

from aerial_control.point_mass import (
    STANDARD_GRAVITY_MPS2,
    point_mass_derivative,
)
from aerial_control.simulation import simulate_fixed_step

MASS_KG = 1.5
MASS_BIAS_KG = 1.65
T0_S = 0.0
DT_S = 0.01

FREE_FALL_TF_S = 2.0
HOVER_TF_S = 10.0
POSITIVE_ROLL_TF_S = 2.0
ROLL_RAD = np.deg2rad(10.0)

POSITION_ATOL_M = 1e-10
VELOCITY_ATOL_MPS = 1e-10

INITIAL_STATE = np.zeros(6, dtype=float)
OUTPUT_DIR = Path(__file__).resolve().parent / "outputs"


def build_cases():
    identity_q_nb = np.array([1.0, 0.0, 0.0, 0.0])
    roll_q_nb = np.array([np.cos(ROLL_RAD / 2.0), np.sin(ROLL_RAD / 2.0), 0.0, 0.0])

    hover_thrust_n = MASS_KG * STANDARD_GRAVITY_MPS2
    positive_roll_thrust_n = hover_thrust_n / np.cos(ROLL_RAD)

    return (
        {
            "slug": "free_fall",
            "display_name": "Free fall",
            "mass_kg": MASS_KG,
            "total_thrust_n": 0.0,
            "q_nb": identity_q_nb.copy(),
            "t0_s": T0_S,
            "tf_s": FREE_FALL_TF_S,
            "dt_s": DT_S,
        },
        {
            "slug": "hover",
            "display_name": "Level hover",
            "mass_kg": MASS_KG,
            "total_thrust_n": hover_thrust_n,
            "q_nb": identity_q_nb.copy(),
            "t0_s": T0_S,
            "tf_s": HOVER_TF_S,
            "dt_s": DT_S,
        },
        {
            "slug": "positive_roll",
            "display_name": "Fixed positive roll",
            "mass_kg": MASS_KG,
            "total_thrust_n": positive_roll_thrust_n,
            "q_nb": roll_q_nb.copy(),
            "t0_s": T0_S,
            "tf_s": POSITIVE_ROLL_TF_S,
            "dt_s": DT_S,
        },
    )


def build_mass_bias_case():
    return {
        "slug": "mass_bias",
        "display_name": "Level attitude with 10% mass increase",
        "mass_kg": MASS_BIAS_KG,
        "total_thrust_n": MASS_KG * STANDARD_GRAVITY_MPS2,
        "q_nb": np.array([1.0, 0.0, 0.0, 0.0]),
        "t0_s": T0_S,
        "tf_s": 2.0,
        "dt_s": DT_S,
    }


def build_analytical_states(case, times):
    times = np.asarray(times, dtype=float)

    if times.ndim != 1:
        raise ValueError("times must be one-dimensional")

    elapsed_s = times - case["t0_s"]
    analytical_states = np.zeros((times.size, 6), dtype=float)

    if case["slug"] == "free_fall":
        # 第 2 列：p_d = 0.5 * g * t^2
        # 第 5 列：v_d = g * t
        analytical_states[:, 2] = 0.5 * STANDARD_GRAVITY_MPS2 * elapsed_s ** 2
        analytical_states[:, 5] = STANDARD_GRAVITY_MPS2 * elapsed_s
    elif case["slug"] == "hover":
        analytical_states[:, :] = 0
    elif case["slug"] == "positive_roll":
        east_acceleration_mps2 = (
            STANDARD_GRAVITY_MPS2 * np.tan(ROLL_RAD)
        )

        # 第 1 列：East 位置
        analytical_states[:, 1] = 0.5 * east_acceleration_mps2 * elapsed_s ** 2

        # 第 4 列：East 速度
        analytical_states[:, 4] = east_acceleration_mps2 * elapsed_s
    elif case["slug"] == "mass_bias":
        down_acceleration_mps2 = (
            STANDARD_GRAVITY_MPS2
            - case["total_thrust_n"] / case["mass_kg"]
        )
        analytical_states[:, 2] = (
            0.5 * down_acceleration_mps2 * elapsed_s ** 2
        )
        analytical_states[:, 5] = down_acceleration_mps2 * elapsed_s
    else:
        raise NotImplementedError(
            f"Analytical solution is not implemented for {case['slug']}"
        )

    return analytical_states


def run_case(case):
    initial_state = INITIAL_STATE.copy()

    def dynamics(t, state):
        return point_mass_derivative(
            t,
            state,
            mass_kg=case["mass_kg"],
            total_thrust_n=case["total_thrust_n"],
            q_nb=case["q_nb"],
        )

    times, numerical_states = simulate_fixed_step(
        dynamics,
        initial_state,
        case["t0_s"],
        case["tf_s"],
        case["dt_s"],
    )

    analytical_states = build_analytical_states(case, times)

    position_abs_error = np.abs(
        numerical_states[:, :3] - analytical_states[:, :3]
    )
    velocity_abs_error = np.abs(
        numerical_states[:, 3:] - analytical_states[:, 3:]
    )

    max_position_error_m = float(np.max(position_abs_error))
    max_velocity_error_mps = float(np.max(velocity_abs_error))
    np.testing.assert_allclose(
        numerical_states[:, :3],
        analytical_states[:, :3],
        rtol=0.0,
        atol=POSITION_ATOL_M,
    )

    np.testing.assert_allclose(
        numerical_states[:, 3:],
        analytical_states[:, 3:],
        rtol=0.0,
        atol=VELOCITY_ATOL_MPS,
    ) 
    return {
        "times": times,
        "numerical_states": numerical_states,
        "analytical_states": analytical_states,
        "position_abs_error": position_abs_error,
        "velocity_abs_error": velocity_abs_error,
        "max_position_error_m": max_position_error_m,
        "max_velocity_error_mps": max_velocity_error_mps,
    }

def save_trajectory(case, result):
    OUTPUT_DIR.mkdir(parents=True, exist_ok=True)
    output_path = OUTPUT_DIR / f"python_{case['slug']}.csv"

    table = np.column_stack(
        (
            result["times"],
            result["numerical_states"],
            result["analytical_states"],
            result["position_abs_error"],
            result["velocity_abs_error"],
        )
    )

    header = ",".join(
        (
            "time_s",
            "num_p_n_m",
            "num_p_e_m",
            "num_p_d_m",
            "num_v_n_mps",
            "num_v_e_mps",
            "num_v_d_mps",
            "ana_p_n_m",
            "ana_p_e_m",
            "ana_p_d_m",
            "ana_v_n_mps",
            "ana_v_e_mps",
            "ana_v_d_mps",
            "abs_err_p_n_m",
            "abs_err_p_e_m",
            "abs_err_p_d_m",
            "abs_err_v_n_mps",
            "abs_err_v_e_mps",
            "abs_err_v_d_mps",
        )
    )

    np.savetxt(
        output_path,
        table,
        delimiter=",",
        header=header,
        comments="",
        fmt="%.17g",
    )
    return output_path

def print_summary(case, result):
    numerical_final = ", ".join(
        f"{value:.17g}" for value in result["numerical_states"][-1]
    )
    analytical_final = ", ".join(
        f"{value:.17g}" for value in result["analytical_states"][-1]
    )
    q_nb_text = ", ".join(f"{value:.17g}" for value in case["q_nb"])

    print(f"Case: {case['display_name']} ({case['slug']})")
    print(
        f"Parameters: mass={case['mass_kg']:.17g} kg, "
        f"thrust={case['total_thrust_n']:.17g} N, "
        f"q_nb=[{q_nb_text}], dt={case['dt_s']:.17g} s"
    )
    print(f"Final time: {result['times'][-1]:.17g} s")
    print(
        "Numerical final state [p_n,p_e,p_d,v_n,v_e,v_d]: "
        f"[{numerical_final}]"
    )
    print(
        "Analytical final state [p_n,p_e,p_d,v_n,v_e,v_d]: "
        f"[{analytical_final}]"
    )
    print(
        f"Maximum position error: "
        f"{result['max_position_error_m']:.17g} m"
    )
    print(
        f"Maximum velocity error: "
        f"{result['max_velocity_error_mps']:.17g} m/s"
    )
    print()


def main():
    parser = argparse.ArgumentParser(
        description="Run the fixed-attitude point-mass demonstrations."
    )
    mass_bias_group = parser.add_mutually_exclusive_group()
    mass_bias_group.add_argument(
        "--include-mass-bias",
        action="store_true",
        help=(
            "also run the 10%% mass-increase case after recording the "
            "required prediction"
        ),
    )
    mass_bias_group.add_argument(
        "--mass-bias-only",
        action="store_true",
        help="run only the 10%% mass-increase case",
    )
    args = parser.parse_args()

    print("Coordinate convention: NED world and FRD body; +z is Down.")
    print("q_NB maps body B to NED N; Python quaternion order is [w,x,y,z].")
    print("Thrust uses T >= 0 with FRD force vector [0,0,-T].")
    print("Integrator: simulate_fixed_step with classical rk4_step.")
    print("All trajectories use double precision and integer step indexing.\n")

    cases = [] if args.mass_bias_only else list(build_cases())
    if args.include_mass_bias or args.mass_bias_only:
        cases.append(build_mass_bias_case())

    for case in cases:
        result = run_case(case)
        output_path = save_trajectory(case, result)
        print_summary(case, result)
        print(f"Trajectory CSV: {output_path}\n")


if __name__ == "__main__":
    main()
