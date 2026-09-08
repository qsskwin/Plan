import numpy as np
from aerial_control.integrators import rk4_step

def oscillator_derivative(t, state):
    return np.array([state[1], -state[0]])
    # state 的顺序是 [q, v]
    # 返回状态导数 [v, -q]

def rk4_step_independent(derivative, t, state, dt):
    k1 = derivative(t, state)
    k2 = derivative(t + dt / 2, state + dt / 2 * k1)
    k3 = derivative(t + dt / 2, state + dt / 2 * k2)
    k4 = derivative(t + dt, state + dt * k3)
    # 依次计算 k1、k2、k3、k4
    # 返回经过一个 dt 后的新状态
    return state + dt / 6 * (k1 + 2 * k2 + 2 * k3 + k4)

def simulate(step_function):
    initial_state = np.array([1.0, 0.0])  # 初始状态 [q, v]
    dt = 0.1  # 时间步长
    t = 0.0  # 初始时间
    num_steps = 10
    for _ in range(num_steps):  # 使用下划线做丢弃的循环变量，不要复用step_function
        initial_state = step_function(oscillator_derivative, t, initial_state, dt)
        t += dt
    return initial_state  # return挪到循环外面

analytical = np.array([np.cos(1.0), -np.sin(1.0)])


independent_result = simulate(rk4_step_independent)
official_result = simulate(rk4_step)
analytical_result = np.array([np.cos(1.0), -np.sin(1.0)])
print("Independent RK4 result:", independent_result)
print("Official RK4 result:", official_result)
print("Analytical result:", analytical_result)
print(np.max(np.abs(independent_result - analytical)))
print(np.max(np.abs(independent_result - official_result)))
