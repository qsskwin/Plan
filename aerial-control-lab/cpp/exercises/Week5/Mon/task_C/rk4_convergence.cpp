#include "core/integrators.hpp"
#include <cmath>
#include <iomanip>
#include <iostream>

aerial_control::StateVector oscillatorDerivative(
    double /*t*/,
    const aerial_control::StateVector& state) {
    aerial_control::StateVector derivative(2);

    derivative << state[1], -state[0];

    return derivative;
}

aerial_control::StateVector simulateOscillator(double dt) {
    constexpr double finalTime = 2.0;

    aerial_control::StateVector state(2);
    state << 1.0, 0.0;

    const int stepCount =
        static_cast<int>(std::round(finalTime / dt));

    for (int step = 0; step < stepCount; ++step) {
        const double t = static_cast<double>(step) * dt;

        state = aerial_control::rk4Step(
            oscillatorDerivative,
            t,
            state,
            dt);
    }

    return state;
}

double oscillatorError(double dt) {
    const aerial_control::StateVector numerical =
        simulateOscillator(dt);

    aerial_control::StateVector analytical(2);
    analytical << std::cos(2.0), -std::sin(2.0);

    return (numerical - analytical).norm();
}

int main() {
    const double error02 = oscillatorError(0.2);
    const double error01 = oscillatorError(0.1);
    const double error005 = oscillatorError(0.05);

    std::cout << std::setprecision(17);
    std::cout << "dt=0.2, error=" << error02 << '\n';
    std::cout << "dt=0.1, error=" << error01 << '\n';
    std::cout << "dt=0.05, error=" << error005 << '\n';
    std::cout << "ratio 0.2/0.1=" << error02 / error01 << '\n';
    std::cout << "ratio 0.1/0.05=" << error01 / error005 << '\n';

    return 0;
}