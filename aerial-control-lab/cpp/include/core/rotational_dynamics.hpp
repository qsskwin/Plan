#pragma once

#include "core/integrators.hpp"

#include <Eigen/Core>

namespace aerial_control
{

  struct RotationalParameters
  {
    Eigen::Vector3d principalInertiaKgM2;
  };

  struct RotationalInput
  {
    Eigen::Vector3d momentBodyNm;
  };

  /// Computes body-frame angular acceleration for a rigid body about its center of mass.
  /// Body axes coincide with principal inertia axes. State order is [omega_x, omega_y, omega_z].
  /// State units are rad/s; the returned three-element derivative has units of rad/s^2.
  /// Rejects invalid dimensions, nonfinite inputs, nonpositive principal inertias, and nonfinite results.
  /// Does not modify inputs, normalize angular rates, or advance the state.
  StateVector angularRateDerivative(double timeS, const StateVector &angularRateBodyRadps, const RotationalParameters &parameters, const RotationalInput &input);

} // namespace aerial_control
