from pathlib import Path

import matplotlib.pyplot as plt
import numpy as np

from quaternion_practice import (
    quaternion_multiply,
    quaternion_normalize,
    quaternion_rotate_vector,
)


def zyx_rotation_matrix(roll: float, pitch: float, yaw: float) -> np.ndarray:
    """Return R_NB = Rz(yaw) @ Ry(pitch) @ Rx(roll)."""
    c_phi, s_phi = np.cos(roll), np.sin(roll)
    c_theta, s_theta = np.cos(pitch), np.sin(pitch)
    c_psi, s_psi = np.cos(yaw), np.sin(yaw)

    r_x = np.array(
        [
            [1.0, 0.0, 0.0],
            [0.0, c_phi, -s_phi],
            [0.0, s_phi, c_phi],
        ]
    )
    r_y = np.array(
        [
            [c_theta, 0.0, s_theta],
            [0.0, 1.0, 0.0],
            [-s_theta, 0.0, c_theta],
        ]
    )
    r_z = np.array(
        [
            [c_psi, -s_psi, 0.0],
            [s_psi, c_psi, 0.0],
            [0.0, 0.0, 1.0],
        ]
    )
    return r_z @ r_y @ r_x


def zyx_quaternion(roll: float, pitch: float, yaw: float) -> np.ndarray:
    """Return q_NB for the same ZYX attitude as zyx_rotation_matrix."""
    q_x = np.array([np.cos(roll / 2), np.sin(roll / 2), 0.0, 0.0])
    q_y = np.array([np.cos(pitch / 2), 0.0, np.sin(pitch / 2), 0.0])
    q_z = np.array([np.cos(yaw / 2), 0.0, 0.0, np.sin(yaw / 2)])

    q_nb = quaternion_multiply(quaternion_multiply(q_z, q_y), q_x)
    return quaternion_normalize(q_nb)


def draw_axes(
    ax: plt.Axes,
    rotated_axes: np.ndarray,
    title: str,
) -> None:
    colors = ("tab:red", "tab:green", "tab:blue")
    axis_names = ("x", "y", "z")
    original_axes = np.eye(3)

    for index, (color, name) in enumerate(zip(colors, axis_names)):
        original = original_axes[:, index]
        rotated = rotated_axes[:, index]

        ax.plot(
            [0.0, original[0]],
            [0.0, original[1]],
            [0.0, original[2]],
            color=color,
            linestyle="--",
            linewidth=1.5,
            alpha=0.55,
            label=f"{name} before",
        )
        ax.plot(
            [0.0, rotated[0]],
            [0.0, rotated[1]],
            [0.0, rotated[2]],
            color=color,
            linewidth=3.0,
            marker="o",
            markevery=[-1],
            label=f"{name} after",
        )

    ax.set_title(title)
    ax.set_xlabel("$x_N$ (North)")
    ax.set_ylabel("$y_N$ (East)")
    ax.set_zlabel("$z_N$ (Down)")
    ax.set_xlim(-1.1, 1.1)
    ax.set_ylim(-1.1, 1.1)
    ax.set_zlim(1.1, -1.1)
    ax.set_box_aspect((1.0, 1.0, 1.0))
    ax.view_init(elev=24, azim=25)
    ax.grid(True, alpha=0.3)
    ax.legend(loc="upper left", fontsize=8)


def main() -> None:
    roll, pitch, yaw = np.deg2rad([30.0, 20.0, 45.0])
    body_axes = np.eye(3)

    r_nb = zyx_rotation_matrix(roll, pitch, yaw)
    q_nb = zyx_quaternion(roll, pitch, yaw)

    axes_from_matrix = r_nb @ body_axes
    axes_from_quaternion = np.column_stack(
        [quaternion_rotate_vector(q_nb, body_axes[:, i]) for i in range(3)]
    )

    max_error = np.max(np.abs(axes_from_matrix - axes_from_quaternion))
    np.testing.assert_allclose(
        axes_from_matrix,
        axes_from_quaternion,
        atol=1e-12,
        rtol=0.0,
    )

    np.set_printoptions(precision=6, suppress=True)
    print("Euler angles [roll, pitch, yaw] = [30, 20, 45] deg")
    print("R_NB =\n", r_nb)
    print("q_NB [w, x, y, z] =", q_nb)
    print("Rotated axes from matrix =\n", axes_from_matrix)
    print("Rotated axes from quaternion =\n", axes_from_quaternion)
    print(f"Maximum absolute difference = {max_error:.3e}")

    figure = plt.figure(figsize=(12, 6))
    matrix_ax = figure.add_subplot(1, 2, 1, projection="3d")
    quaternion_ax = figure.add_subplot(1, 2, 2, projection="3d")

    draw_axes(matrix_ax, axes_from_matrix, "Rotation matrix: $R_{NB}$")
    draw_axes(quaternion_ax, axes_from_quaternion, "Unit quaternion: $q_{NB}$")
    figure.suptitle("Same ZYX attitude: roll=30°, pitch=20°, yaw=45°")
    figure.tight_layout()

    output_path = Path(__file__).with_name("zyx_attitude_comparison.png")
    figure.savefig(output_path, dpi=180, bbox_inches="tight")
    print("Saved figure to", output_path)
    plt.show()


if __name__ == "__main__":
    main()
