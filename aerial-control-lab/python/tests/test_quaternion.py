import numpy as np
import pytest

from aerial_control.quaternion import (
    quat_conjugate,
    quat_multiply,
    quat_normalize,
    quat_rotate_vector,
)


def axis_angle_quaternion(axis, angle_rad):
    unit_axis = np.asarray(axis, dtype=float)
    unit_axis /= np.linalg.norm(unit_axis)
    half_angle = angle_rad / 2.0
    return np.concatenate(
        ([np.cos(half_angle)], unit_axis * np.sin(half_angle))
    )


def test_identity_quaternion_does_not_change_vector():
    vector = np.array([1.5, -2.0, 4.25])
    np.testing.assert_allclose(
        quat_rotate_vector([1.0, 0.0, 0.0, 0.0], vector),
        vector,
        atol=1e-12,
    )


def test_quarter_turn_about_down_axis_rotates_north_to_east():
    q_nb = axis_angle_quaternion([0.0, 0.0, 1.0], np.pi / 2.0)
    np.testing.assert_allclose(
        quat_rotate_vector(q_nb, [1.0, 0.0, 0.0]),
        [0.0, 1.0, 0.0],
        atol=1e-12,
    )


def test_rotation_preserves_vector_norm():
    quaternion = axis_angle_quaternion([1.0, -2.0, 0.5], 1.37)
    vector = np.array([3.0, -4.0, 2.5])
    rotated = quat_rotate_vector(quaternion, vector)
    assert np.linalg.norm(rotated) == pytest.approx(
        np.linalg.norm(vector), abs=1e-12
    )


def test_rotation_followed_by_inverse_recovers_original_vector():
    quaternion = axis_angle_quaternion([0.3, 0.7, -0.2], -0.83)
    vector = np.array([-1.0, 2.5, 0.75])
    rotated = quat_rotate_vector(quaternion, vector)
    recovered = quat_rotate_vector(quat_conjugate(quaternion), rotated)
    np.testing.assert_allclose(recovered, vector, atol=1e-12)


def test_composed_rotation_matches_hamilton_product_order():
    q_x = axis_angle_quaternion([1.0, 0.0, 0.0], np.pi / 2.0)
    q_z = axis_angle_quaternion([0.0, 0.0, 1.0], np.pi / 2.0)
    vector = np.array([0.0, 1.0, 0.0])

    sequential = quat_rotate_vector(q_z, quat_rotate_vector(q_x, vector))
    composed = quat_rotate_vector(quat_multiply(q_z, q_x), vector)

    np.testing.assert_allclose(composed, sequential, atol=1e-12)
    np.testing.assert_allclose(composed, [0.0, 0.0, 1.0], atol=1e-12)


def test_non_unit_quaternion_normalizes_to_unit_norm():
    normalized = quat_normalize([2.0, -2.0, 1.0, 3.0])
    assert np.linalg.norm(normalized) == pytest.approx(1.0, abs=1e-12)


def test_quaternion_and_negation_rotate_vector_identically():
    quaternion = axis_angle_quaternion([0.0, 1.0, 0.0], 0.61)
    vector = np.array([1.0, 2.0, 3.0])
    np.testing.assert_allclose(
        quat_rotate_vector(quaternion, vector),
        quat_rotate_vector(-quaternion, vector),
        atol=1e-12,
    )


def test_conjugate_product_equals_squared_norm_identity():
    quaternion = np.array([2.0, -1.0, 3.0, 0.5])
    product = quat_multiply(quaternion, quat_conjugate(quaternion))
    expected = np.array([np.dot(quaternion, quaternion), 0.0, 0.0, 0.0])
    np.testing.assert_allclose(product, expected, atol=1e-12)


@pytest.mark.parametrize(
    ("function", "arguments"),
    [
        (quat_normalize, ([0.0, 0.0, 0.0, 0.0],)),
        (quat_normalize, ([1.0, 0.0, 0.0],)),
        (quat_conjugate, ([1.0, 0.0, np.inf, 0.0],)),
        (quat_rotate_vector, ([1.0, 0.0, 0.0, 0.0], [1.0, 2.0])),
    ],
)
def test_invalid_inputs_raise_value_error(function, arguments):
    with pytest.raises(ValueError):
        function(*arguments)
