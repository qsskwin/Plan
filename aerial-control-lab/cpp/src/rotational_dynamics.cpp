#include "core/rotational_dynamics.hpp"

#include <Eigen/Geometry>

#include <cmath>
#include <stdexcept>

namespace aerial_control
{

  StateVector angularRateDerivative(double timeS, const StateVector &angularRateBodyRadps, 
                                    const RotationalParameters &parameters, const RotationalInput &input)
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

    // 将角速度状态读取为固定三维向量。
    const Eigen::Vector3d omegaBodyRadps = angularRateBodyRadps;
    // 主惯量与角速度逐分量相乘，得到机体系角动量。
    const Eigen::Vector3d angularMomentumBodyKgM2ps = parameters.principalInertiaKgM2.cwiseProduct(omegaBodyRadps);
    // 计算角速度与角动量的叉积，得到耦合项。
    const Eigen::Vector3d couplingMomentBodyNm = omegaBodyRadps.cross(angularMomentumBodyKgM2ps);
    // 外力矩减去耦合项，再逐分量除以主惯量，得到角加速度。
    const StateVector angularAccelerationBodyRadps2 = (input.momentBodyNm - couplingMomentBodyNm).cwiseQuotient(parameters.principalInertiaKgM2);

    if (!angularAccelerationBodyRadps2.allFinite())
    {
      throw std::invalid_argument("angular acceleration must be finite");
    }

    return angularAccelerationBodyRadps2;
  }

} // namespace aerial_control
