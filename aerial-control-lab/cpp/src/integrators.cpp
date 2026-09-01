#include "core/integrators.hpp"

#include <cmath>
#include <stdexcept>
#include <string>

namespace aerial_control {
namespace {

void validateState(const StateVector& state, const char* argument_name) {
    if (state.size() == 0) {
        throw std::invalid_argument(
            std::string{argument_name} + " must not be empty");
    }
    if (!state.allFinite()) {
        throw std::invalid_argument(
            std::string{argument_name} +
            " must contain only finite values");
    }
}

void validateInputs(
    const DerivativeFunction& derivative,
    const StateVector& state,
    const double dt) {
    if (!derivative) {
        throw std::invalid_argument("derivative must be callable");
    }
    if (!std::isfinite(dt) || dt <= 0.0) {
        throw std::invalid_argument("dt must be positive and finite");
    }
    validateState(state, "state");
}

StateVector evaluateDerivative(
    const DerivativeFunction& derivative,
    const double t,
    const StateVector& state) {
    validateState(state, "evaluation state");
    StateVector value = derivative(t, state);

    if (value.size() != state.size()) {
        throw std::invalid_argument(
            "derivative must return the same dimension as state");
    }
    if (!value.allFinite()) {
        throw std::invalid_argument(
            "derivative must return only finite values");
    }
    return value;
}

StateVector validateResult(StateVector result) {
    if (!result.allFinite()) {
        throw std::invalid_argument(
            "integrated state must contain only finite values");
    }
    return result;
}

}  // namespace

StateVector eulerStep(
    const DerivativeFunction& derivative,
    const double t,
    const StateVector& state,
    const double dt) {
    validateInputs(derivative, state, dt);

    const StateVector slope = evaluateDerivative(derivative, t, state);
    return validateResult(state + dt * slope);
}

StateVector rk4Step(
    const DerivativeFunction& derivative,
    const double t,
    const StateVector& state,
    const double dt) {
    validateInputs(derivative, state, dt);

    const double half_dt = 0.5 * dt;
    const StateVector k1 = evaluateDerivative(derivative, t, state);
    const StateVector k2 = evaluateDerivative(
        derivative, t + half_dt, state + half_dt * k1);
    const StateVector k3 = evaluateDerivative(
        derivative, t + half_dt, state + half_dt * k2);
    const StateVector k4 = evaluateDerivative(
        derivative, t + dt, state + dt * k3);

    return validateResult(
        state + (dt / 6.0) * (k1 + 2.0 * k2 + 2.0 * k3 + k4));
}

}  // namespace aerial_control
