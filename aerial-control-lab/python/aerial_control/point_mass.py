import numpy as np

from aerial_control.quaternion import quat_rotate_vector


STANDARD_GRAVITY_MPS2 = 9.80665


def point_mass_derivative(
    t,
    state,
    *,
    mass_kg,
    total_thrust_n,
    q_nb,
):
    """Compute the derivative of the six-dimensional NED point-mass state."""

    # TODO 1: 将 state 转成浮点 NumPy 数组
    state = np.asarray(state, dtype=float)
    # TODO 2: 检查 state 的形状和有限性
    if state.shape != (6,):
        raise ValueError("State must be a 6D vector [x, y, z, vx, vy, vz]")
    if not np.all(np.isfinite(state)):
        raise ValueError("State must contain only finite values")
    # TODO 3: 检查 mass_kg
    if mass_kg <= 0 or not np.isfinite(mass_kg):
        raise ValueError("Mass must be a positive value")
    # TODO 4: 检查 total_thrust_n   
    if total_thrust_n < 0 or not np.isfinite(total_thrust_n):
        raise ValueError("Total thrust must be a non-negative value")
    # TODO 5: 构造 FRD 推力向量
    thrust_frd = np.array([0, 0, -total_thrust_n])
    # TODO 6: 使用 quat_rotate_vector 转换到 NED
    thrust_ned = quat_rotate_vector(q_nb, thrust_frd)
    # TODO 7: 计算 NED 加速度
    acceleration_ned = thrust_ned / mass_kg + np.array([0, 0, STANDARD_GRAVITY_MPS2])
    # TODO 8: 拼出 [v_n, v_e, v_d, a_n, a_e, a_d]
    derivative = np.concatenate([state[3:6], acceleration_ned])
    return derivative