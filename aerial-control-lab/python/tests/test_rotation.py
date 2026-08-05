import numpy as np
import pytest

from aerial_control.rotation import rot_x, rot_y, rot_z


ROTATION_FUNCTIONS = (rot_x, rot_y, rot_z)


@pytest.mark.parametrize("rotation", ROTATION_FUNCTIONS)
def test_zero_angle_returns_identity(rotation):
    np.testing.assert_allclose(rotation(0.0), np.eye(3), atol=1e-12)


@pytest.mark.parametrize(
    ("rotation", "source", "expected"),
    [
        (rot_x, [0.0, 1.0, 0.0], [0.0, 0.0, 1.0]),
        (rot_y, [0.0, 0.0, 1.0], [1.0, 0.0, 0.0]),
        (rot_z, [1.0, 0.0, 0.0], [0.0, 1.0, 0.0]),
    ],
)
def test_positive_quarter_turn_follows_right_hand_rule(rotation, source, expected):
    rotated = rotation(np.pi / 2.0) @ np.asarray(source)
    np.testing.assert_allclose(rotated, expected, atol=1e-12)


@pytest.mark.parametrize(
    ("rotation", "expected"),
    [
        (rot_x, np.diag([1.0, -1.0, -1.0])),
        (rot_y, np.diag([-1.0, 1.0, -1.0])),
        (rot_z, np.diag([-1.0, -1.0, 1.0])),
    ],
)
def test_half_turn_has_expected_matrix(rotation, expected):
    np.testing.assert_allclose(rotation(np.pi), expected, atol=1e-12)


@pytest.mark.parametrize("rotation", ROTATION_FUNCTIONS)
@pytest.mark.parametrize("angle", [-2.4, -0.5, 0.3, 1.7, np.pi])
def test_rotation_is_orthogonal_with_positive_determinant(rotation, angle):
    matrix = rotation(angle)
    np.testing.assert_allclose(matrix.T @ matrix, np.eye(3), atol=1e-12)
    assert np.linalg.det(matrix) == pytest.approx(1.0, abs=1e-12)


@pytest.mark.parametrize("rotation", ROTATION_FUNCTIONS)
def test_single_axis_rotation_preserves_vector_norm(rotation):
    vector = np.array([1.5, -2.0, 4.25])
    rotated = rotation(0.73) @ vector
    assert np.linalg.norm(rotated) == pytest.approx(np.linalg.norm(vector), abs=1e-12)


def test_random_composed_rotations_with_fixed_seed():
    generator = np.random.default_rng(20260805)

    for _ in range(32):
        roll, pitch, yaw = generator.uniform(-np.pi, np.pi, size=3)
        vector = generator.normal(size=3)
        combined = rot_z(yaw) @ rot_y(pitch) @ rot_x(roll)

        np.testing.assert_allclose(combined.T @ combined, np.eye(3), atol=1e-12)
        assert np.linalg.det(combined) == pytest.approx(1.0, abs=1e-12)
        assert np.linalg.norm(combined @ vector) == pytest.approx(
            np.linalg.norm(vector), abs=1e-12
        )
