#include "control_utils.hpp"

#include <stdexcept>

namespace aerial_control::week2_mon {

double ClampValue(double value, double lower_bound, double upper_bound) {
  if (lower_bound > upper_bound) {
    throw std::invalid_argument("lower_bound must not exceed upper_bound");
  }

  if (value < lower_bound) {
    return lower_bound;
  }

  if (value > upper_bound) {
    return upper_bound;
  }

  return value;
}

bool IsFiniteVector(const Eigen::Vector3d& vector) {
  return vector.allFinite();
}

}  // namespace aerial_control::week2_mon
