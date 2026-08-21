from free_fall_euler import simulate_free_fall, analytical_free_fall, free_fall_derivative, euler_step
import numpy as np
import pytest
def test_free_fall_derivative_at_initial_state():
    np.testing.assert_allclose(
        free_fall_derivative(0, np.array([0, 0])),
        np.array([0, 9.80665])
    )

def test_euler_step_from_rest():

    np.testing.assert_allclose(
        euler_step(free_fall_derivative, 0, np.array([0, 0]), 0.1),
        np.array([0, 0.980665])
    )

def test_simulate_free_fall_final_state():
    times,states = simulate_free_fall(0.1)
    np.testing.assert_allclose(
        states[-1,1],
        19.6133,
    )

def test_position_error_decreases_with_smaller_dt():
    states_1 = simulate_free_fall(0.1)[1]
    states_2 = simulate_free_fall(0.05)[1]
    assert abs(19.6133 - states_1[-1,0]) > abs(states_2[-1,0] - 19.6133)

def test_euler_step_rejects_zero_dt():
    with pytest.raises(ValueError):
        euler_step(free_fall_derivative, 0, np.array([0, 0]), 0.0)

def test_simulate_free_fall_returns_finite_values():
    times, states = simulate_free_fall(0.01)
    assert np.isfinite(times).all()
    assert np.isfinite(states).all()