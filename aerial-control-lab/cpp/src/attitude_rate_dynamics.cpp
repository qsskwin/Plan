#include "core/attitude_rate_dynamics.hpp"

#include "core/attitude_kinematics.hpp"

#include <stdexcept>

namespace aerial_control
{

  StateVector attitudeRateDerivative(double timeS, const StateVector &state,
                                    const RotationalParameters &parameters, const RotationalInput &input)
  {
    if (state.size() != 7)
    {
      throw std::invalid_argument("state must have exactly 7 elements");
    }

    StateVector quaternionWxyz = state.head<4>();
    StateVector angularRateBodyRadps = state.tail<3>();
    StateVector quaternionDerivative = attitudeKinematicsDerivative(timeS, quaternionWxyz, angularRateBodyRadps);
    StateVector angularAccelerationBodyRadps2 = angularRateDerivative(timeS, angularRateBodyRadps, parameters, input);

    StateVector derivative(7);
    derivative.head<4>() = quaternionDerivative;
    derivative.tail<3>() = angularAccelerationBodyRadps2;

    if (!derivative.allFinite())
    {
      throw std::invalid_argument("attitude rate derivative must be finite");
    }

    return derivative;
  }

} // namespace aerial_control
