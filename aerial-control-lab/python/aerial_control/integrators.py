"""General-purpose one-step numerical integrators."""

from collections.abc import Callable
from numbers import Real

import numpy as np


Derivative = Callable[[float, np.ndarray], np.ndarray]


def _validate_state(state: np.ndarray) -> None:
    if not isinstance(state, np.ndarray):
        raise TypeError("state must be a NumPy array")
    if state.ndim != 1:
        raise ValueError("state must be one-dimensional")
    if state.size == 0:
        raise ValueError("state must not be empty")
    if not np.issubdtype(state.dtype, np.number):
        raise TypeError("state must contain numeric values")
    if not np.all(np.isfinite(state)):
        raise ValueError("state must contain only finite values")


def _validate_dt(dt: float) -> float:
    if isinstance(dt, (bool, np.bool_)) or not isinstance(dt, Real):
        raise TypeError("dt must be a real number")

    dt_value = float(dt)
    if not np.isfinite(dt_value) or dt_value <= 0.0:
        raise ValueError("dt must be positive and finite")
    return dt_value


def _evaluate_derivative(
    derivative: Derivative, t: float, state: np.ndarray
) -> np.ndarray:
    _validate_state(state)
    value = derivative(t, state.copy())

    if not isinstance(value, np.ndarray):
        raise TypeError("derivative must return a NumPy array")
    if value.shape != state.shape:
        raise ValueError("derivative must return the same shape as state")
    if not np.issubdtype(value.dtype, np.number):
        raise TypeError("derivative must return numeric values")
    if not np.all(np.isfinite(value)):
        raise ValueError("derivative must return only finite values")
    return value


def _validate_result(state: np.ndarray) -> None:
    if not np.all(np.isfinite(state)):
        raise ValueError("integrated state must contain only finite values")


def euler_step(
    derivative: Derivative, t: float, state: np.ndarray, dt: float
) -> np.ndarray:
    """Advance a state by one explicit Euler step."""
    if not callable(derivative):
        raise TypeError("derivative must be callable")
    _validate_state(state)
    dt_value = _validate_dt(dt)

    current = state.copy()
    slope = _evaluate_derivative(derivative, t, current)
    with np.errstate(over="ignore", invalid="ignore"):
        result = current + dt_value * slope
    _validate_result(result)
    return result


def rk4_step(
    derivative: Derivative, t: float, state: np.ndarray, dt: float
) -> np.ndarray:
    """Advance a state by one classical fourth-order Runge-Kutta step."""
    if not callable(derivative):
        raise TypeError("derivative must be callable")
    _validate_state(state)
    dt_value = _validate_dt(dt)

    current = state.copy()
    half_dt = 0.5 * dt_value

    k1 = _evaluate_derivative(derivative, t, current)
    with np.errstate(over="ignore", invalid="ignore"):
        midpoint_from_k1 = current + half_dt * k1
    k2 = _evaluate_derivative(derivative, t + half_dt, midpoint_from_k1)

    with np.errstate(over="ignore", invalid="ignore"):
        midpoint_from_k2 = current + half_dt * k2
    k3 = _evaluate_derivative(derivative, t + half_dt, midpoint_from_k2)

    with np.errstate(over="ignore", invalid="ignore"):
        endpoint_from_k3 = current + dt_value * k3
    k4 = _evaluate_derivative(derivative, t + dt_value, endpoint_from_k3)

    with np.errstate(over="ignore", invalid="ignore"):
        result = current + (dt_value / 6.0) * (k1 + 2.0 * k2 + 2.0 * k3 + k4)
    _validate_result(result)
    return result
