from numbers import Real

import numpy as np

from aerial_control.integrators import rk4_step


def simulate_fixed_step(derivative, initial_state, t0, tf, dt):
    """
    Simulate a system using a fixed-step integration method.

    Parameters:
    - derivative: A function that computes the derivative of the state.
    - initial_state: The initial state of the system.
    - t0: The initial time.
    - tf: The final time.
    - dt: The time step for integration.

    Returns:
    - times: A one-dimensional NumPy array of time points.
    - states: A two-dimensional NumPy array of state history.
    """
    if not callable(derivative):
        raise TypeError("derivative must be callable")

    initial = np.asarray(initial_state, dtype=float).copy()
    if initial.ndim != 1:
        raise ValueError("Initial state must be a 1D array")
    if initial.size == 0:
        raise ValueError("Initial state must not be empty")
    if not np.all(np.isfinite(initial)):
        raise ValueError("Initial state must contain only finite values")

    time_values = {"t0": t0, "tf": tf, "dt": dt}
    for name, value in time_values.items():
        if isinstance(value, (bool, np.bool_)) or not isinstance(value, Real):
            raise TypeError(f"{name} must be a real number")
        if not np.isfinite(value):
            raise ValueError(f"{name} must be finite")

    t0_value = float(t0)
    tf_value = float(tf)
    dt_value = float(dt)
    if dt_value <= 0.0:
        raise ValueError("dt must be positive")
    if tf_value <= t0_value:
        raise ValueError("tf must be greater than t0")

    duration = tf_value - t0_value
    step_ratio = duration / dt_value
    num_steps = int(round(step_ratio))
    if not np.isclose(
        num_steps * dt_value,
        duration,
        rtol=1e-12,
        atol=1e-12,
    ):
        raise ValueError("The time interval must be divisible by dt")

    times = t0_value + np.arange(num_steps + 1, dtype=float) * dt_value
    times[-1] = tf_value
    states = np.empty((num_steps + 1, initial.size), dtype=float)
    states[0] = initial

    # TODO: integration loop
    for k in range(num_steps):
        states[k + 1] = rk4_step(
            derivative,
            times[k],
            states[k],
            dt_value,
        )
    return times, states