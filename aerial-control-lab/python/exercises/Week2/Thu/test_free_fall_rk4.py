import numpy as np
import pytest

from free_fall_euler import (
    analytical_free_fall,
    free_fall_derivative,
    rk4_step,
    simulate_free_fall,
    simulate_free_fall_rk4,
)


def test_rk4_step_matches_constant_acceleration_solution():
    state = rk4_step(free_fall_derivative, 0.0, np.array([0.0, 0.0]), 0.1)
    np.testing.assert_allclose(state, np.array([0.04903325, 0.980665]))


def test_rk4_rejects_nonpositive_dt():
    with pytest.raises(ValueError, match="dt must be positive"):
        rk4_step(free_fall_derivative, 0.0, np.array([0.0, 0.0]), 0.0)


def test_rk4_final_state_matches_analytical_solution():
    times, states = simulate_free_fall_rk4(0.1)
    positions, velocities = analytical_free_fall(times)
    np.testing.assert_allclose(states[:, 0], positions, atol=1e-12)
    np.testing.assert_allclose(states[:, 1], velocities, atol=1e-12)


def test_rk4_position_error_is_smaller_than_euler_error():
    times, euler_states = simulate_free_fall(0.1)
    _, rk4_states = simulate_free_fall_rk4(0.1)
    analytical_positions, _ = analytical_free_fall(times)

    euler_error = abs(euler_states[-1, 0] - analytical_positions[-1])
    rk4_error = abs(rk4_states[-1, 0] - analytical_positions[-1])
    assert rk4_error < euler_error
