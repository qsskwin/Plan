#include "control_utils.hpp"

#include <iostream>

#include <Eigen/Core>

int main() {
  const double clamped_value = aerial_control::week2_mon::ClampValue(3.0, 0.0, 2.0);

  const Eigen::Vector3d vector(1.0, -2.0, 3.0);
  const bool vector_is_finite = aerial_control::week2_mon::IsFiniteVector(vector);

  std::cout << "Clamped value: " << clamped_value << '\n';
  std::cout << "Vector is finite: "<< (vector_is_finite ? "true" : "false") << '\n';

  return 0;
}