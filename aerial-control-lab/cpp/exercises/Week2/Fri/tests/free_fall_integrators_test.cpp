#include "free_fall_integrators.hpp"

#include <gtest/gtest.h>

#include <cmath>

#include <stdexcept>

TEST(FreeFallDerivativeTest, ReturnsVelocityAndConstantGravity) {
    week2::fri::State state;
    state(0) = 12.0;
    state(1) = -3.5;

    const week2::fri::State derivative =
        week2::fri::free_fall_derivative(7.0, state);

    EXPECT_DOUBLE_EQ(derivative(0), state(1));
    EXPECT_DOUBLE_EQ(derivative(1), week2::fri::kGravity);
}

TEST(EulerStepTest, AdvancesOneStepFromRest) {
    constexpr double dt = 0.1;
    const week2::fri::State initial_state =
        week2::fri::State::Zero();

    const week2::fri::State next_state =
        week2::fri::euler_step(0.0, initial_state, dt);

    EXPECT_DOUBLE_EQ(next_state(0), 0.0);
    EXPECT_NEAR(next_state(1), week2::fri::kGravity * dt, 1e-12);
}

TEST(Rk4StepTest, MatchesAnalyticalSolutionForOneStep) {
    constexpr double dt = 0.1;

    const week2::fri::State initial_state =
        week2::fri::State::Zero();

    const week2::fri::State next_state =
        week2::fri::rk4_step(0.0, initial_state, dt);

    const double expected_position =
        0.5 * week2::fri::kGravity * dt * dt;
    const double expected_velocity =
        week2::fri::kGravity * dt;

    EXPECT_NEAR(next_state(0), expected_position, 1e-12);
    EXPECT_NEAR(next_state(1), expected_velocity, 1e-12);
}

TEST(FreeFallSimulationTest, BothMethodsMatchFinalVelocityAtTwoSeconds) {
    constexpr double dt = 0.1;
    constexpr double end_time = 2.0;
    constexpr int number_of_steps = 20;

    week2::fri::State euler_state =
        week2::fri::State::Zero();
    week2::fri::State rk4_state =
        week2::fri::State::Zero();

    double t = 0.0;

    for (int step = 0; step < number_of_steps; ++step) {
        euler_state =
            week2::fri::euler_step(t, euler_state, dt);
        rk4_state =
            week2::fri::rk4_step(t, rk4_state, dt);

        t = static_cast<double>(step + 1) * dt;
    }

    const double expected_velocity =
        week2::fri::kGravity * end_time;

    EXPECT_NEAR(euler_state(1), expected_velocity, 1e-12);
    EXPECT_NEAR(rk4_state(1), expected_velocity, 1e-12);
}

TEST(IntegratorAccuracyTest,
     Rk4PositionErrorIsSmallerThanEulerAtTwoSeconds) {
    constexpr double dt = 0.1;
    constexpr double end_time = 2.0;
    constexpr int number_of_steps = 20;

    week2::fri::State euler_state =
        week2::fri::State::Zero();
    week2::fri::State rk4_state =
        week2::fri::State::Zero();

    double t = 0.0;

    for (int step = 0; step < number_of_steps; ++step) {
        euler_state =
            week2::fri::euler_step(t, euler_state, dt);
        rk4_state =
            week2::fri::rk4_step(t, rk4_state, dt);

        t = static_cast<double>(step + 1) * dt;
    }

    const double expected_position =
        0.5 * week2::fri::kGravity * end_time * end_time;

    const double euler_position_error =
        std::abs(euler_state(0) - expected_position);

    const double rk4_position_error =
        std::abs(rk4_state(0) - expected_position);

    EXPECT_LT(rk4_position_error, euler_position_error);
    EXPECT_NEAR(rk4_position_error, 0.0, 1e-12);
}

TEST(IntegratorValidationTest, RejectsNonpositiveDt) {
    const week2::fri::State state =
        week2::fri::State::Zero();

    EXPECT_THROW(
        week2::fri::euler_step(0.0, state, 0.0),
        std::invalid_argument
    );

    EXPECT_THROW(
        week2::fri::euler_step(0.0, state, -0.1),
        std::invalid_argument
    );

    EXPECT_THROW(
        week2::fri::rk4_step(0.0, state, 0.0),
        std::invalid_argument
    );

    EXPECT_THROW(
        week2::fri::rk4_step(0.0, state, -0.1),
        std::invalid_argument
    );
}

TEST(IntegratorOutputTest, RemainsFiniteDuringSimulation) {
    constexpr double dt = 0.1;
    constexpr int number_of_steps = 20;

    week2::fri::State euler_state =
        week2::fri::State::Zero();
    week2::fri::State rk4_state =
        week2::fri::State::Zero();

    double t = 0.0;

    for (int step = 0; step < number_of_steps; ++step) {
        euler_state =
            week2::fri::euler_step(t, euler_state, dt);
        rk4_state =
            week2::fri::rk4_step(t, rk4_state, dt);

        t = static_cast<double>(step + 1) * dt;

        ASSERT_TRUE(std::isfinite(euler_state(0)));
        ASSERT_TRUE(std::isfinite(euler_state(1)));
        ASSERT_TRUE(std::isfinite(rk4_state(0)));
        ASSERT_TRUE(std::isfinite(rk4_state(1)));
    }
}