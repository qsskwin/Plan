#pragma once

#include <Eigen/Core>

namespace week2::fri {

    using State = Eigen::Vector2d;

    inline constexpr double kGravity = 9.80665;  // m/s^2, NED down is positive

    State free_fall_derivative(double t, const State& state);

    State euler_step(double t, const State& state, double dt);

    State rk4_step(double t, const State& state, double dt);
}   // namespace week2::fri

