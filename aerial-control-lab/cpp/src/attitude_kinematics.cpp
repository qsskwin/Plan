#include "core/attitude_kinematics.hpp"

#include <Eigen/Geometry>

#include <cmath>
#include <stdexcept>

namespace aerial_control
{

  StateVector attitudeKinematicsDerivative(double timeS, const StateVector &quaternionWxyz, const Eigen::Vector3d &angularRateBodyRadps)
  {
    if (quaternionWxyz.size() != 4)
    {
      throw std::invalid_argument("quaternionWxyz must have exactly 4 elements");
    }

    if (!std::isfinite(timeS) || !quaternionWxyz.allFinite() || !angularRateBodyRadps.allFinite())
    {
      throw std::invalid_argument("attitude kinematics inputs must be finite");
    }

    if (quaternionWxyz.norm() == 0.0)
    {
      throw std::invalid_argument("quaternionWxyz must have a nonzero norm");
    }

    const double w = quaternionWxyz(0);
    const Eigen::Vector3d qv = quaternionWxyz.tail<3>();

    const double wDot = -0.5 * qv.dot(angularRateBodyRadps);
    const Eigen::Vector3d qvDot = 0.5 * (w * angularRateBodyRadps + qv.cross(angularRateBodyRadps));

    StateVector derivative(4);
    derivative << wDot, qvDot(0), qvDot(1), qvDot(2);
    return derivative;
  }

} // namespace aerial_control
