#pragma once

#include <Eigen/Geometry>

#include "core/integrators.hpp"

namespace aerial_control {

inline constexpr double kStandardGravityMps2 = 9.80665;

struct PointMassParameters {
    double massKg;
};

struct PointMassInput {
    double totalThrustN;
    Eigen::Quaterniond qNb;
};

StateVector pointMassDerivative(
    double t,
    const StateVector& state,
    const PointMassParameters& parameters,
    const PointMassInput& input);

}  // namespace aerial_control
