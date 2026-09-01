import numpy as np

phi, theta, psi = np.deg2rad([20.0, -15.0, 35.0])

c_phi = np.cos(phi)
s_phi = np.sin(phi)
c_theta = np.cos(theta)
s_theta = np.sin(theta)
c_psi = np.cos(psi)
s_psi = np.sin(psi)

R_x = np.array([
    [1.0, 0.0, 0.0],
    [0.0, c_phi, -s_phi],
    [0.0, s_phi, c_phi],
])

R_y = np.array([
    [c_theta, 0.0, s_theta],
    [0.0, 1.0, 0.0],
    [-s_theta, 0.0, c_theta],
])

R_z = np.array([
    [c_psi, -s_psi, 0.0],
    [s_psi, c_psi, 0.0],
    [0.0, 0.0, 1.0],    
])

R_nb = R_z @ R_y @ R_x
v_b = np.array([1.0, 2.0, 3.0])
v_direct = R_nb @ v_b
v_sequential = R_z @ (R_y @ (R_x @ v_b))
np.set_printoptions(precision=6, suppress=True)
print("R_nb =")
print(R_nb)

print("v_direct =")
print(v_direct)
print("v_sequential =")
print(v_sequential)
print("Are they close?", np.allclose(v_direct, v_sequential))