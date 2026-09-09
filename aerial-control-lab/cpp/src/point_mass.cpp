#include "core/point_mass.hpp"
#include "core/rotation.hpp"

#include <cmath>
#include <stdexcept>

namespace aerial_control {

StateVector pointMassDerivative(
    double t,
    const StateVector& state,
    const PointMassParameters& parameters,
    const PointMassInput& input) {
    (void)t;
    if (state.size() != 6) {
        throw std::invalid_argument("State vector must have size 6.");
    }
    if (state.allFinite() == false) {
        throw std::invalid_argument(
            "State vector must have finite values.");
    }

    if (parameters.massKg <= 0.0 ||
        std::isfinite(parameters.massKg) == false) {
        throw std::invalid_argument("Mass must be positive and finite.");
    }

    if (input.totalThrustN < 0.0 ||
        std::isfinite(input.totalThrustN) == false) {
        throw std::invalid_argument(
            "Total thrust must be non-negative and finite.");
    }

    const Eigen::Vector3d thrust_frd{
        0.0,
        0.0,
        -input.totalThrustN,
    };

    const Eigen::Vector3d thrust_ned =
        rotateBodyToNed(input.qNb, thrust_frd);

    const Eigen::Vector3d gravity_ned{
        0.0,
        0.0,
        kStandardGravityMps2,
    };

    const Eigen::Vector3d acceleration_ned =
        thrust_ned / parameters.massKg + gravity_ned;

    StateVector derivative(6);
    derivative.head<3>() = state.tail<3>();  // 读取输入状态最后三项
    derivative.tail<3>() = acceleration_ned;  // 写入 a_n, a_e, a_d
    return derivative;
}

}  // namespace aerial_control
