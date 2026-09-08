import numpy as np

from aerial_control.quaternion import quat_rotate_vector
from aerial_control.rotation import rot_x


def main():
    # 1. NED -> ENU -> NED，并检查旋转前后的模长。
    ned = np.array([1.0, 2.0, 3.0])
    enu = np.array([ned[1], ned[0], -ned[2]])
    recovered_ned = np.array([enu[1], enu[0], -enu[2]])
    ned_round_trip_error = np.max(np.abs(recovered_ned - ned))
    ned_enu_norm_difference = abs(np.linalg.norm(enu) - np.linalg.norm(ned))

    # 2. 用 R_NB 正向旋转，再用 R_NB^T 恢复原机体系向量。
    roll_rad = np.deg2rad(10.0)
    r_nb = rot_x(roll_rad)
    vector_body = np.array([0.4, -1.2, 2.3])
    vector_ned = r_nb @ vector_body
    recovered_body = r_nb.T @ vector_ned
    transpose_recovery_error = np.max(np.abs(recovered_body - vector_body))

    # 3. 正滚转时，FRD 负 z 推力应产生 NED 正 East 分量。
    q_nb = np.array(
        [
            np.cos(roll_rad / 2),
            np.sin(roll_rad / 2),
            0.0,
            0.0,
        ]
    )
    thrust_body = np.array([0.0, 0.0, -1.0])
    thrust_ned = quat_rotate_vector(q_nb, thrust_body)
    expected_thrust_ned = np.array(
        [0.0, np.sin(roll_rad), -np.cos(roll_rad)]
    )
    thrust_direction_error = np.max(
        np.abs(thrust_ned - expected_thrust_ned)
    )

    print("Original NED:", ned)
    print("Converted ENU:", enu)
    print("Recovered NED:", recovered_ned)
    print("NED round-trip max error:", ned_round_trip_error)
    print("NED/ENU norm difference:", ned_enu_norm_difference)
    print()
    print("Original body vector:", vector_body)
    print("Rotated NED vector:", vector_ned)
    print("Recovered body vector using R_NB.T:", recovered_body)
    print("Transpose recovery max error:", transpose_recovery_error)
    print()
    print("Positive-roll thrust in NED:", thrust_ned)
    print("Expected thrust in NED:", expected_thrust_ned)
    print("Thrust direction max error:", thrust_direction_error)


if __name__ == "__main__":
    main()
