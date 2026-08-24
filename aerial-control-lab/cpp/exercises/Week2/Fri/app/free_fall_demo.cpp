#include "free_fall_integrators.hpp"

#include <iomanip>
#include <iostream>

int main() {
    constexpr double dt = 0.1;
    constexpr double end_time = 2.0;
    constexpr int number_of_steps = 20;

    week2::fri::State euler_state = week2::fri::State::Zero();
    week2::fri::State rk4_state = week2::fri::State::Zero();
    double t = 0.0;

    for (int step = 0; step < number_of_steps; ++step) {
        euler_state = week2::fri::euler_step(t, euler_state, dt);
        rk4_state = week2::fri::rk4_step(t, rk4_state, dt);
        t = static_cast<double>(step + 1) * dt;
    }


    const double analytical_position = week2::fri::kGravity * end_time * end_time / 2.0;
    const double analytical_velocity = week2::fri::kGravity * end_time;

    std::cout << std::fixed << std::setprecision(9);

    std::cout << "Time: " << t << " s\n";

    std::cout << "Euler [z, v_z]: ["
            << euler_state(0) << ", "
            << euler_state(1) << "]\n";

    std::cout << "RK4 [z, v_z]: ["
            << rk4_state(0) << ", "
            << rk4_state(1) << "]\n";
            
    std::cout << "Analytical position: " << analytical_position << std::endl;
    std::cout << "Analytical velocity: " << analytical_velocity << std::endl;
    return 0;
}