import math
import numpy as np
drone_count = 3
print(drone_count)
height = 5.0
print(height)
is_flying = True
print(is_flying)
is_above_3m = height > 3.0
print(is_above_3m)
def double_value(value):
    return value * 2

doubled_height = double_value(height)
print(doubled_height)

def add_values(value1, value2):
    return value1 + value2

added_height = add_values(height, 2.0)
print(added_height) 
values = [1, 2, 3, 4, 5]
print(values)
print(values[0])
print(values[-1])

for value in values:
    print(value)

print(math.sqrt(16.0))

print(__name__)== "__main__"
height_array = np.array([0.5, 1.0, 1.5])

matrix_a = np.array([[1, 2], [3, 4]])
print(matrix_a)
print(matrix_a.shape)
print(matrix_a.dtype)
print(matrix_a[0, 0])
print(matrix_a[0, 1])
print(matrix_a[1, 0])
print(matrix_a[1, 1])

print(matrix_a * matrix_a)
print(matrix_a @ matrix_a)

if __name__ == "__main__":
    print("python_basics.py is running directly")

