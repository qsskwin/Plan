"""Right-handed three-dimensional rotation matrices for column vectors."""

from __future__ import annotations

import numpy as np

__all__ = ["rot_x", "rot_y", "rot_z"]


def rot_x(angle_rad: float) -> np.ndarray:
    """Return the active right-handed rotation about the x-axis."""
    angle = float(angle_rad)
    cosine = np.cos(angle)
    sine = np.sin(angle)
    return np.array(
        [
            [1.0, 0.0, 0.0],
            [0.0, cosine, -sine],
            [0.0, sine, cosine],
        ],
        dtype=float,
    )


def rot_y(angle_rad: float) -> np.ndarray:
    """Return the active right-handed rotation about the y-axis."""
    angle = float(angle_rad)
    cosine = np.cos(angle)
    sine = np.sin(angle)
    return np.array(
        [
            [cosine, 0.0, sine],
            [0.0, 1.0, 0.0],
            [-sine, 0.0, cosine],
        ],
        dtype=float,
    )


def rot_z(angle_rad: float) -> np.ndarray:
    """Return the active right-handed rotation about the z-axis."""
    angle = float(angle_rad)
    cosine = np.cos(angle)
    sine = np.sin(angle)
    return np.array(
        [
            [cosine, -sine, 0.0],
            [sine, cosine, 0.0],
            [0.0, 0.0, 1.0],
        ],
        dtype=float,
    )
