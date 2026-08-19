import numpy as np


def quaternion_conjugate(q: np.ndarray) -> np.ndarray:
    q_1 = np.ndarray.copy(q)
    q_1[1:] *= -1
    return q_1

def quaternion_multiply(
    q1: np.ndarray, q2: np.ndarray
) -> np.ndarray:
    w1, x1, y1, z1 = q1
    w2, x2, y2, z2 = q2
    w = w1 * w2 - x1 * x2 - y1 * y2 - z1 * z2
    x = w1 * x2 + x1 * w2 + y1 * z2 - z1 * y2
    y = w1 * y2 - x1 * z2 + y1 * w2 + z1 * x2
    z = w1 * z2 + x1 * y2 - y1 * x2 + z1 * w2
    return np.array([w, x, y, z])

def quaternion_normalize(q: np.ndarray) -> np.ndarray:
    q_norm = np.linalg.norm(q)
    if q_norm == 0:
        raise ValueError("Cannot normalize a zero quaternion.") 
    return q / q_norm

def quaternion_rotate_vector(
    q: np.ndarray, v: np.ndarray
) -> np.ndarray:
    q_unit = quaternion_normalize(q)
    p = np.array([0.0, v[0] , v[1], v[2]])
    q_temp = quaternion_multiply(q_unit, p)
    p_rotated = quaternion_multiply(q_temp, quaternion_conjugate(q_unit))
    return p_rotated[1:]  # Return the vector part of the quaternion

