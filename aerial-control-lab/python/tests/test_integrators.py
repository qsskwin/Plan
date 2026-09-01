import numpy as np
import pytest

from aerial_control.integrators import euler_step, rk4_step


INTEGRATORS = (euler_step, rk4_step)


def oscillator_derivative(t, state):
    del t
    return np.array([state[1], -state[0]])


def simulate_oscillator(step, dt, final_time=2.0):
    state = np.array([1.0, 0.0])
    step_count = int(round(final_time / dt))

    for index in range(step_count):
        state = step(oscillator_derivative, index * dt, state, dt)
    return state


def oscillator_error(state, t):
    exact = np.array([np.cos(t), -np.sin(t)])
    return np.linalg.norm(state - exact)


def test_euler_step_updates_state_from_current_slope():
    state = np.array([1.0, 0.0])

    result = euler_step(oscillator_derivative, 0.0, state, 0.1)

    np.testing.assert_allclose(result, [1.0, -0.1])


def test_rk4_step_matches_oscillator_solution_for_one_small_step():
    state = np.array([1.0, 0.0])

    result = rk4_step(oscillator_derivative, 0.0, state, 0.1)

    expected = np.array([np.cos(0.1), -np.sin(0.1)])
    np.testing.assert_allclose(result, expected, atol=1e-7, rtol=0.0)


def test_rk4_step_uses_intermediate_times():
    def time_dependent_derivative(t, state):
        return np.full_like(state, t)

    result = rk4_step(
        time_dependent_derivative, 0.0, np.array([0.0]), 0.2
    )

    np.testing.assert_allclose(result, [0.02], atol=1e-15, rtol=0.0)


@pytest.mark.parametrize("step", INTEGRATORS)
def test_integrator_integrates_constant_derivative(step):
    def constant_derivative(t, state):
        del t, state
        return np.array([2.0, -1.0])

    result = step(
        constant_derivative, 0.0, np.array([1.0, 3.0]), 0.1
    )

    np.testing.assert_allclose(result, [1.2, 2.9], atol=1e-15, rtol=0.0)


def test_rk4_error_is_smaller_than_euler_at_same_step_size():
    dt = 0.1
    final_time = 2.0

    euler_error = oscillator_error(
        simulate_oscillator(euler_step, dt, final_time), final_time
    )
    rk4_error = oscillator_error(
        simulate_oscillator(rk4_step, dt, final_time), final_time
    )

    assert rk4_error < euler_error


@pytest.mark.parametrize(
    ("step", "coarse_dt"),
    [(euler_step, 0.2), (rk4_step, 0.4)],
    ids=("euler", "rk4"),
)
def test_integrator_error_falls_when_step_size_is_halved(step, coarse_dt):
    final_time = 2.0
    coarse_error = oscillator_error(
        simulate_oscillator(step, coarse_dt, final_time), final_time
    )
    fine_error = oscillator_error(
        simulate_oscillator(step, coarse_dt / 2.0, final_time), final_time
    )

    assert fine_error < coarse_error


@pytest.mark.parametrize("step", INTEGRATORS)
def test_integrator_does_not_modify_input_state(step):
    state = np.array([1.0, 0.0])
    original = state.copy()

    step(oscillator_derivative, 0.0, state, 0.1)

    np.testing.assert_array_equal(state, original)


@pytest.mark.parametrize("step", INTEGRATORS)
@pytest.mark.parametrize(
    "invalid_state",
    [
        np.array([]),
        np.array([[1.0, 0.0]]),
        np.array([np.nan, 0.0]),
        np.array([np.inf, 0.0]),
    ],
    ids=("empty", "two-dimensional", "nan", "infinite"),
)
def test_integrator_rejects_invalid_state(step, invalid_state):
    with pytest.raises(ValueError):
        step(oscillator_derivative, 0.0, invalid_state, 0.1)


@pytest.mark.parametrize("step", INTEGRATORS)
def test_integrator_requires_numpy_state(step):
    with pytest.raises(TypeError):
        step(oscillator_derivative, 0.0, [1.0, 0.0], 0.1)


@pytest.mark.parametrize("step", INTEGRATORS)
@pytest.mark.parametrize("invalid_dt", [0.0, -0.1, np.inf, -np.inf, np.nan])
def test_integrator_rejects_non_positive_or_non_finite_dt(step, invalid_dt):
    with pytest.raises(ValueError):
        step(oscillator_derivative, 0.0, np.array([1.0, 0.0]), invalid_dt)


@pytest.mark.parametrize("step", INTEGRATORS)
def test_integrator_rejects_derivative_with_wrong_shape(step):
    def wrong_shape_derivative(t, state):
        del t, state
        return np.zeros(3)

    with pytest.raises(ValueError):
        step(wrong_shape_derivative, 0.0, np.array([1.0, 0.0]), 0.1)


@pytest.mark.parametrize("step", INTEGRATORS)
@pytest.mark.parametrize("invalid_value", [np.nan, np.inf])
def test_integrator_rejects_non_finite_derivative(step, invalid_value):
    def non_finite_derivative(t, state):
        del t
        return np.full_like(state, invalid_value)

    with pytest.raises(ValueError):
        step(non_finite_derivative, 0.0, np.array([1.0, 0.0]), 0.1)


@pytest.mark.parametrize("step", INTEGRATORS)
def test_integrator_requires_numpy_derivative_result(step):
    def list_derivative(t, state):
        del t, state
        return [0.0, -1.0]

    with pytest.raises(TypeError):
        step(list_derivative, 0.0, np.array([1.0, 0.0]), 0.1)
