#include "core/attitude_rate_dynamics.hpp"

#include "core/attitude_kinematics.hpp"

#include <stdexcept>

namespace aerial_control
{

  StateVector attitudeRateDerivative(double timeS, const StateVector &state,
                                    const RotationalParameters &parameters, const RotationalInput &input)
  {
    // TODO: Check that state has exactly seven elements before reading its blocks.

    // Placeholder while the derivative is being implemented in small steps.
    throw std::logic_error("attitudeRateDerivative is not implemented yet");
  }

} // namespace aerial_control
