import numpy as np
import matplotlib.pyplot as plt

original_vector = np.array([1.0, 0.0])
print("Original vector:", original_vector)
angle_degrees = -45.0
angle_radians = np.deg2rad(angle_degrees)
rotation_matrix = np.array([ [np.cos(angle_radians), -np.sin(angle_radians)], 
                            [np.sin(angle_radians), np.cos(angle_radians)] ])
print(rotation_matrix)
print(rotation_matrix.shape)

rotated_vector = rotation_matrix @ original_vector
print("Rotated vector:", rotated_vector)
fig, ax = plt.subplots()
print(type(fig))
print(type(ax))

ax.quiver(
    0.0,
    0.0,
    original_vector[0],
    original_vector[1],
    angles="xy",
    scale_units="xy",
    scale=1,
    color="tab:blue",
    label="Original vector",
)

ax.quiver(
    0.0,
    0.0,
    rotated_vector[0],
    rotated_vector[1],
    angles="xy",
    scale_units="xy",
    scale=1,
    color="tab:orange",
    label="Rotated vector",
)

ax.set_xlim(-1.2, 1.2)
ax.set_ylim(-1.2, 1.2)
ax.set_aspect("equal", adjustable="box")

ax.axhline(0.0, color="black", linewidth=0.8)
ax.axvline(0.0, color="black", linewidth=0.8)
ax.grid(True, linestyle="--", alpha=0.4)

ax.set_xlabel("x displacement [m]")
ax.set_ylabel("y displacement [m]")

ax.set_title("Original and Rotated Vectors")
ax.legend()

fig.tight_layout()
fig.savefig("vector_rotation.png", dpi=150, bbox_inches="tight")


plt.show()