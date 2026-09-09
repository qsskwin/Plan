
import numpy as np
from aerial_control.simulation import simulate_fixed_step
from aerial_control.point_mass import (
    STANDARD_GRAVITY_MPS2,
    point_mass_derivative,
)
mass_kg = 1.5
q_nb = [1, 0, 0, 0]
total_thrust_n = mass_kg * STANDARD_GRAVITY_MPS2
initial_state = np.zeros(6)
t0 = 0
tf = 10
dt = 0.01


def hover_dynamics(t, state):
    return point_mass_derivative(
        t,
        state,
        mass_kg = mass_kg,
        total_thrust_n=total_thrust_n,
        q_nb = q_nb,
    )


initial_state_before = initial_state.copy()
times, states = simulate_fixed_step(
    hover_dynamics,
    initial_state,
    t0,
    tf,
    dt,
)

assert times.shape == (1001,)
assert states.shape == (1001, 6)
assert times[0] == t0
assert times[-1] == tf
assert np.all(np.isfinite(times))
assert np.all(np.isfinite(states))
np.testing.assert_array_equal(initial_state, initial_state_before)

print(f"Update count: {times.size - 1}")
print(f"State count: {times.size}")
print(f"Final time: {times[-1]:.2f} s")
print(f"Final state: {states[-1]}")

# 数值轨迹：前三列是 N/E/D 位置，后三列是 N/E/D 速度
numerical_position = states[:, :3]
numerical_velocity = states[:, 3:]

# 构造每个记录时刻的三维解析轨迹
analytical_position = np.zeros_like(numerical_position)
analytical_velocity = np.zeros_like(numerical_velocity)

# 分别计算误差，不能把 m 和 m/s 合并
position_abs_error = np.abs(numerical_position - analytical_position)
velocity_abs_error = np.abs(numerical_velocity - analytical_velocity)

max_position_error_m = np.max(position_abs_error)
max_velocity_error_mps = np.max(velocity_abs_error)

np.testing.assert_allclose(
    numerical_position,
    analytical_position,
    atol=1e-10,
    rtol=0.0,
)

np.testing.assert_allclose(
    numerical_velocity,
    analytical_velocity,
    atol=1e-10,
    rtol=0.0,
)

print(f"Maximum position error: {max_position_error_m:.3e} m")
print(f"Maximum velocity error: {max_velocity_error_mps:.3e} m/s")