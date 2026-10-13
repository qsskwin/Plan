#pragma once

#include "core/integrators.hpp"
#include "core/rotational_dynamics.hpp"

namespace aerial_control
{

  /// Computes the seven-element quaternion and body angular-rate derivative.
  /// State order is [q_w, q_x, q_y, q_z, omega_x, omega_y, omega_z].
  /// Both derivatives use the same evaluation state; the quaternion may be non-unit.
  /// Returns a new derivative vector without modifying or normalizing the input state.
  StateVector attitudeRateDerivative(double timeS, const StateVector &state,
                                    const RotationalParameters &parameters, const RotationalInput &input);

} // namespace aerial_control
