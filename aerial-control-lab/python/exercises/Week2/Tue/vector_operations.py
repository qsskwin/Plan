import numpy as np


def normalize_vector(vector):
    norm = np.linalg.norm(vector)
    if norm == 0:
        raise ValueError("cannot normalize a zero vector")
    return vector / norm


vector_x = np.array([1.0, 0.0, 0.0])
vector_y = np.array([0.0, 1.0, 0.0])
print(vector_x + vector_y)
print(vector_x - vector_y)
print(vector_x * vector_y)
print(vector_x @ vector_y)
print(np.dot(vector_x, vector_y))
print(np.cross(vector_x, vector_y))
print(np.cross(vector_y, vector_x))
vector_a = np.array([3.0, 4.0, 0.0])
print(np.linalg.norm(vector_a))
unit_vector_a = normalize_vector(vector_a)
print(unit_vector_a)
print(np.linalg.norm(unit_vector_a))
vector_0 = np.array([0, 0, 0])
print(np.linalg.norm(vector_0))
# unit_vector_0 = normalize_vector(vector_0)
# print(unit_vector_0)

rotation_matrix = np.array([[0, -1, 0], [1, 0, 0], [0, 0, 1]])
print(rotation_matrix.shape)

print(rotation_matrix @ vector_x)
print(rotation_matrix.T @ vector_x)

