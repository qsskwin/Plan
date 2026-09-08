"""Quaternion operations using the scalar-first Hamilton convention."""

from __future__ import annotations

from collections.abc import Sequence

import numpy as np

__all__ = [
    "quat_conjugate",
    "quat_multiply",
    "quat_normalize",
    "quat_rotate_vector",
] 
# `__all__`：控制 `from xxx import *` 时导出哪些公开 API。

def _as_finite_vector(
    value: Sequence[float] | np.ndarray,
    *,
    size: int,
    name: str,
) -> np.ndarray:
    array = np.asarray(value, dtype=float)
    if array.shape != (size,):
        raise ValueError(f"{name} must have shape ({size},), got {array.shape}")
    if not np.all(np.isfinite(array)):
        raise ValueError(f"{name} must contain only finite values")
    return array
# `*` 之后的参数**必须使用关键字传参，不能按位置传参**。强制可读性，防止参数顺序搞错。

def quat_normalize(quaternion: Sequence[float] | np.ndarray) -> np.ndarray:
    """Return a unit copy of a scalar-first quaternion ``[w, x, y, z]``.

    Raises:
        ValueError: If the input has the wrong shape, contains a non-finite
            value, or has zero norm.
    """

    q = _as_finite_vector(quaternion, size=4, name="quaternion")
    norm = float(np.linalg.norm(q))
    if norm <= np.finfo(float).eps:
        raise ValueError("cannot normalize a zero-norm quaternion")
    return q / norm

#  四元数共轭
def quat_conjugate(quaternion: Sequence[float] | np.ndarray) -> np.ndarray:
    """Return the conjugate ``[w, -x, -y, -z]`` of a quaternion."""

    q = _as_finite_vector(quaternion, size=4, name="quaternion")
    conjugate = q.copy()
    conjugate[1:] *= -1.0
    return conjugate

# 哈密顿乘积 
def quat_multiply(
    left: Sequence[float] | np.ndarray,
    right: Sequence[float] | np.ndarray,
) -> np.ndarray:
    """Return the Hamilton product ``left ⊗ right`` in scalar-first order."""

    p = _as_finite_vector(left, size=4, name="left quaternion")
    q = _as_finite_vector(right, size=4, name="right quaternion")
    pw, px, py, pz = p
    qw, qx, qy, qz = q

    return np.array(
        [
            pw * qw - px * qx - py * qy - pz * qz,
            pw * qx + px * qw + py * qz - pz * qy,
            pw * qy - px * qz + py * qw + pz * qx,
            pw * qz + px * qy - py * qx + pz * qw,
        ],
        dtype=float,
    )

## `quat_rotate_vector` 四元数旋转三维矢量 就是四元数如何旋转三维向量的。 q x v x q* 其中 q* 是 q 的共轭。
def quat_rotate_vector(
    quaternion: Sequence[float] | np.ndarray,
    vector: Sequence[float] | np.ndarray,
) -> np.ndarray:
    """Rotate a 3-vector with ``q ⊗ [0, v] ⊗ q*``.

    The input quaternion is normalized first so a finite non-unit quaternion
    still represents a pure rotation. A zero quaternion is rejected.
    """

    unit_q = quat_normalize(quaternion)
    v = _as_finite_vector(vector, size=3, name="vector") 
    pure_vector = np.concatenate(([0.0], v)) # 将三维向量 v 转换为纯四元数 [0, v]
    rotated = quat_multiply(
        quat_multiply(unit_q, pure_vector),
        quat_conjugate(unit_q),
    ) 
    return rotated[1:]
