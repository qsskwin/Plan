import numpy as np
import pytest
from vector_operations import vector_x, vector_y, normalize_vector, rotation_matrix


def test_orthogonal_vectors_have_zero_dot_product():
    result = np.dot(vector_x, vector_y)
    assert result == 0.0

def test_vector_addition():
    actual = vector_x + vector_y
    expected = np.array([1.0, 1.0, 0.0])
    np.testing.assert_allclose(actual, expected)

def test_normalize_nonzero_vector():
    actual = normalize_vector(np.array([3.0, 4.0, 0.0]))
    expected = np.array([0.6, 0.8, 0.0])
    np.testing.assert_allclose(actual, expected)

def test_normalize_zero_vector_raises_value_error():
    with pytest.raises(ValueError):
        normalize_vector(np.array([0.0, 0.0, 0.0]))

def test_rotation_matrix_maps_x_to_y():
    actual = rotation_matrix @ vector_x
    expected = vector_y
    np.testing.assert_allclose(actual, expected)