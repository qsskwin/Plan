#include "free_fall_integrators.hpp"

#include <stdexcept>

namespace week2::fri {

State free_fall_derivative(double t, const State& state) {
    static_cast<void>(t);
    State derivative;
    derivative(0) = state(1);
    derivative(1) = kGravity;

    return derivative;
}

State euler_step(double t, const State& state, double dt) {
    if (!(dt > 0.0)) {
        throw std::invalid_argument("dt must be positive");
    }
    State next_state = state + dt * free_fall_derivative(t, state);
    return next_state;
}

State rk4_step(double t, const State& state, double dt) {
    if (!(dt > 0.0)) {
        throw std::invalid_argument("dt must be positive");
    }

    const State k1 = free_fall_derivative(t, state);
    const State k2 =
        free_fall_derivative(t + 0.5 * dt, state + 0.5 * dt * k1);
    const State k3 =
        free_fall_derivative(t + 0.5 * dt, state + 0.5 * dt * k2);
    const State k4 = free_fall_derivative(t + dt, state + dt * k3);

    return state +
           (dt / 6.0) * (k1 + 2.0 * k2 + 2.0 * k3 + k4);
}

}  // namespace week2::fri
