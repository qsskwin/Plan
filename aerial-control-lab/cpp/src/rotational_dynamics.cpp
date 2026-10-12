#include "core/rotational_dynamics.hpp"

#include <Eigen/Geometry>

#include <cmath>
#include <stdexcept>

namespace aerial_control
{

  StateVector angularRateDerivative(double timeS, const StateVector &angularRateBodyRadps, const RotationalParameters &parameters, const RotationalInput &input)
  {
    if (angularRateBodyRadps.size() != 3)
    {
      throw std::invalid_argument("angularRateBodyRadps must have exactly 3 elements");
    }

    if (!std::isfinite(timeS) || !angularRateBodyRadps.allFinite() || !input.momentBodyNm.allFinite())
    {
      throw std::invalid_argument("time, angular rate, and moment must be finite");
    }

    if (!parameters.principalInertiaKgM2.allFinite() || (parameters.principalInertiaKgM2.array() <= 0.0).any())
    {
      throw std::invalid_argument("principal inertias must be positive and finite");
    }

    const Eigen::Vector3d omegaBodyRadps = angularRateBodyRadps;
    const Eigen::Vector3d angularMomentumBodyKgM2ps = parameters.principalInertiaKgM2.cwiseProduct(omegaBodyRadps);
    const Eigen::Vector3d couplingMomentBodyNm = omegaBodyRadps.cross(angularMomentumBodyKgM2ps);
    const StateVector angularAccelerationBodyRadps2 = (input.momentBodyNm - couplingMomentBodyNm).cwiseQuotient(parameters.principalInertiaKgM2);

    if (!angularAccelerationBodyRadps2.allFinite())
    {
      throw std::invalid_argument("angular acceleration must be finite");
    }

    return angularAccelerationBodyRadps2;
  }

} // namespace aerial_control
