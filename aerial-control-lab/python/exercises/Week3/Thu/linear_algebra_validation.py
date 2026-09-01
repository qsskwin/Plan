import numpy as np

A = np.array([[2, 1],[1, 2]])
eigenvalues, eigenvectors = np.linalg.eigh(A)

eigenvalue_1 = eigenvalues[0]
eigenvector_1 = eigenvectors[:, 0]

av = A @ eigenvector_1
lambda_v = eigenvalue_1 * eigenvector_1

print("Av =", av)
print("lambda*v =", lambda_v)

rng = np.random.default_rng(20260827)
x = rng.random(2)
x = x[:, np.newaxis]   # 升级为列向量 shape=(2,1)

print("x =\n", x)
print("x.TAx =", x.T @ A @ x)

B = np.array([[1, 0],[0, -1]])
print("x.T @ B @ x =", x.T @ B @ x)
x_1 = np.array([[1], [0]])   # 升级为列向量 shape=(2,1)
print("x_1.T @ B @ x_1 =", x_1.T @ B @ x_1)