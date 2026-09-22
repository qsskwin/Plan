#pragma once

#include "core/integrators.hpp"

#include <Eigen/Core>

namespace aerial_control
{

  /// Computes q_NB derivative from the body-frame angular rate.
  /// Quaternion state and derivative use [w, x, y, z] order.
  /// The state may be non-unit during an RK4 intermediate evaluation.
  StateVector attitudeKinematicsDerivative(double timeS, const StateVector &quaternionWxyz, const Eigen::Vector3d &angularRateBodyRadps);

} // namespace aerial_control
