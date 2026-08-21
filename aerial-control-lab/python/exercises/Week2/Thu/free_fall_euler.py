import numpy as np

g = 9.80665

def free_fall_derivative(t, t_state):
    return np.array([t_state[1], g])

def euler_step(derivative_func, t, state, dt):
    if dt <= 0:
        raise ValueError("dt must be positive")
    return state + derivative_func(t, state) * dt

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




def analytical_free_fall(times):
    position = 0.5 * g * times**2
    velocity = g * times
    return position, velocity

if __name__ == "__main__":
    for dt in [0.1, 0.05, 0.01]:
        times, states = simulate_free_fall(dt)
        print(dt, states[-1])