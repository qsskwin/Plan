import numpy as np

g = 9.80665

def free_fall_derivative(t, t_state):
    return np.array([t_state[1], g])

def euler_step(derivative_func, t, state, dt):
    if dt <= 0:
        raise ValueError("dt must be positive")
    return state + derivative_func(t, state) * dt


def rk4_step(derivative_func, t, state, dt):
    if dt <= 0:
        raise ValueError("dt must be positive")

    k1 = derivative_func(t, state)
    k2 = derivative_func(t + 0.5 * dt, state + 0.5 * dt * k1)
    k3 = derivative_func(t + 0.5 * dt, state + 0.5 * dt * k2)
    k4 = derivative_func(t + dt, state + dt * k3)

    return state + (dt / 6.0) * (k1 + 2.0 * k2 + 2.0 * k3 + k4)

def simulate_free_fall(dt, end_time=2.0):
    t = 0.0
    state = np.array([0.0, 0.0])
      # Initial position and velocity
    times = [t]
    states = [state.copy()]
    num_steps = int(round(end_time / dt))
    for i in range(num_steps):
        state = euler_step(free_fall_derivative, t, state, dt)
        t = (i + 1) *   dt
        times.append(t)
        states.append(state.copy())
    return np.array(times), np.array(states)


def simulate_free_fall_rk4(dt, end_time=2.0):
    t = 0.0
    state = np.array([0.0, 0.0])
    times = [t]
    states = [state.copy()]
    num_steps = int(round(end_time / dt))

    for i in range(num_steps):
        state = rk4_step(free_fall_derivative, t, state, dt)
        t = (i + 1) * dt
        times.append(t)
        states.append(state.copy())

    return np.array(times), np.array(states)




def analytical_free_fall(times):
    position = 0.5 * g * times**2
    velocity = g * times
    return position, velocity

if __name__ == "__main__":
    for dt in [0.1, 0.05, 0.01]:
        _, euler_states = simulate_free_fall(dt)
        _, rk4_states = simulate_free_fall_rk4(dt)
        print(f"dt={dt:g}, Euler={euler_states[-1]}, RK4={rk4_states[-1]}")
