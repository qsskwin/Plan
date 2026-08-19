import numpy as np
from quaternion_practice import quaternion_rotate_vector, quaternion_conjugate,quaternion_multiply,quaternion_normalize
import pytest
def test_quaternion_rotate_vector():
    v = np.array([1.0, -2, 3])
    v_o = np.linalg.norm(v)
    q = np.array([np.sqrt(2)/2, 0, 0.0, np.sqrt(2)/2]) 
    v = quaternion_rotate_vector(q, v)
    v_o_rotated = np.linalg.norm(v)
    np.testing.assert_allclose(v, np.array([2.0, 1, 3]))
    np.testing.assert_allclose(v_o, v_o_rotated)

def test_identity_rotation():
    v_original = np.array([1.0, -2.0, 3.0])
    q_identity = np.array([1.0, 0.0, 0.0, 0.0])

    v_rotated = quaternion_rotate_vector(q_identity, v_original)

    np.testing.assert_allclose(v_rotated, v_original)

def test_forward_inverse_recovery():
    q = np.array([np.sqrt(2)/2, 0.0, 0.0, np.sqrt(2)/2])  # 90-degree rotation around z-axis
    v_original = np.array([1.0, 0.0, 0.0])

    v_forward = quaternion_rotate_vector(q, v_original)
    q_inverse = quaternion_conjugate(q)
    v_recovered = quaternion_rotate_vector(q_inverse, v_forward)

    np.testing.assert_allclose(v_recovered, v_original)

def test_composition_of_rotations():
    qz = np.array([np.sqrt(2)/2, 0.0, 0.0, np.sqrt(2)/2])  # 90-degree rotation around z-axis
    qx = np.array([np.sqrt(2)/2, np.sqrt(2)/2, 0.0, 0.0])  # 90-degree rotation around x-axis
    v_original = np.array([1.0, 0.0, 0.0])

    v_rotated_qz = quaternion_rotate_vector(qz, v_original)
    v_rotated_qx = quaternion_rotate_vector(qx, v_rotated_qz)

    q_composed = quaternion_multiply(qx, qz)
    v_rotated_composed = quaternion_rotate_vector(q_composed, v_original)

    np.testing.assert_allclose(v_rotated_qx, v_rotated_composed)
    np.testing.assert_allclose(np.array([0.0, 0.0, 1.0]), v_rotated_composed)   

def test_q_and_neg_q_same_rotation():
    q = np.array([np.sqrt(2)/2, 0.0, 0.0, np.sqrt(2)/2])  # 90-degree rotation around z-axis
    v_original = np.array([1.0, 0.0, 0.0])

    v_rotated = quaternion_rotate_vector(q, v_original)
    v_rotated_neg_q = quaternion_rotate_vector(-q, v_original)
    np.testing.assert_allclose(v_rotated_neg_q, v_rotated)  

def test_normalization_of_quaternion():
    with pytest.raises(ValueError):
        quaternion_normalize(np.zeros(4))   
