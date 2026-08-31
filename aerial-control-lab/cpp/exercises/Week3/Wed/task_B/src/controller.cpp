#include "controller.hpp"

#include <cmath>
#include <stdexcept>

namespace aerial_control::week3_wed {

double ZeroController::compute(double error) const {
  if (!std::isfinite(error)) {
    throw std::invalid_argument("Error must be finite");
  }

  return 0.0;
}

ProportionalController::ProportionalController(double gain) : gain_(gain) {
  if (!std::isfinite(gain)) {
    throw std::invalid_argument("Gain must be finite");
  }
}

double ProportionalController::compute(double error) const {
  if (!std::isfinite(error)) {
    throw std::invalid_argument("Error must be finite");
  }

  return gain_ * error;
}

}  // namespace aerial_control::week3_wed
