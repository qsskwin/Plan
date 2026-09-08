import numpy as np

from aerial_control.point_mass import point_mass_derivative, STANDARD_GRAVITY_MPS2
import pytest

def test_position_derivative_equals_velocity():
    state = np.array(
        [1.0, 2.0, 3.0, 4.0, 5.0, 6.0],
        dtype=float,
    )

    derivative = point_mass_derivative(
        0.0,
        state,
        mass_kg=    1.0,
        total_thrust_n= 0.0,
        q_nb=[1.0, 0.0, 0.0, 0.0],
    )

    assert derivative.shape == (6,)
    np.testing.assert_array_equal(
        derivative[:3],
        state[3:],
    )

def test_zero_thrust_produces_ned_gravity():
    state = np.array(
        [1.0, 2.0, 3.0, 4.0, 5.0, 6.0],
        dtype=float,
    )

    derivative = point_mass_derivative(
        0.0,
        state,
        mass_kg=    1.0,
        total_thrust_n= 0.0,
        q_nb=[1.0, 0.0, 0.0, 0.0],
    )    
    np.testing.assert_allclose(
        derivative[3:],
        [0.0, 0.0, 9.80665],
        atol=1e-12,
        rtol=0.0
    )

def test_hover_thrust_cancels_ned_gravity():
    state = np.array(
        [1.0, 2.0, 3.0, 4.0, 5.0, 6.0],
        dtype=float,
    )

    derivative = point_mass_derivative(
        0.0,
        state,
        mass_kg=    1.5,
        total_thrust_n= STANDARD_GRAVITY_MPS2 * 1.5,
        q_nb=[1.0, 0.0, 0.0, 0.0],
    )    
    np.testing.assert_allclose(
        derivative[3:],
        [0.0, 0.0, 0.0],
        atol=1e-12,
        rtol=0.0
    )

def test_positive_roll_produces_expected_east_acceleration():
    state = np.array(
        [1.0, 2.0, 3.0, 4.0, 5.0, 6.0],
        dtype=float,
    )
    phi = np.deg2rad(10.0)
    mass_kg = 1.0

    # 10 degree roll
    derivative = point_mass_derivative(
        0.0,
        state,
        mass_kg=mass_kg,
        total_thrust_n=mass_kg * STANDARD_GRAVITY_MPS2 / np.cos(phi),
        q_nb=[np.cos(phi / 2.0), np.sin(phi / 2.0), 0.0, 0.0],
    )
    np.testing.assert_allclose(
        derivative[3:6],
        [0.0, STANDARD_GRAVITY_MPS2 * np.tan(phi), 0.0],
        atol=1e-12,
        rtol=0.0,
    )

@pytest.mark.parametrize(
    "invalid_mass",
    [0.0, -1.0, np.nan, np.inf, -np.inf],
)
def test_invalid_mass_is_rejected(invalid_mass):
    state = np.array(
        [1.0, 2.0, 3.0, 4.0, 5.0, 6.0],
        dtype=float,
    )    
    with pytest.raises(ValueError):
        point_mass_derivative(
            0.0,
            state,
            mass_kg=    invalid_mass,
            total_thrust_n= 0.0,
            q_nb=[1.0, 0.0, 0.0, 0.0],
        )    
        # mass_kg 使用 invalid_mass，其余输入必须合法

@pytest.mark.parametrize(
"invalid_thrust",
[-1.0, np.nan, np.inf, -np.inf],
)     
def test_invalid_total_thrust_is_rejected(invalid_thrust):
    state = np.array(
        [1.0, 2.0, 3.0, 4.0, 5.0, 6.0],
        dtype=float,
    )    
    with pytest.raises(ValueError):
        point_mass_derivative(
            0.0,
            state,
            mass_kg=    1.0,
            total_thrust_n= invalid_thrust,
            q_nb=[1.0, 0.0, 0.0, 0.0],
        )    
        # total_thrust_n 使用 invalid_thrust，其余输入必须合法

@pytest.mark.parametrize(
    "invalid_state",
    [
        np.array([1.0, 2.0, 3.0, 4.0, 5.0]),  # Too short
        np.array([1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0]),  # Too long
        np.array([1.0, 2.0, np.nan, 4.0, 5.0, 6.0]),  # Contains NaN
        np.array([1.0, 2.0, np.inf, 4.0, 5.0, 6.0]),  # Contains Inf
        np.array([1.0, 2.0, -np.inf, 4.0, 5.0, 6.0]), # Contains -Inf
    ],
)
def test_invalid_state_is_rejected(invalid_state):
    with pytest.raises(ValueError):
        # 使用 invalid_state 调用 point_mass_derivative
        # 质量、推力和四元数必须合法