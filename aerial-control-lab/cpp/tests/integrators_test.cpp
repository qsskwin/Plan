#include <gtest/gtest.h>

#include <cmath>
#include <limits>
#include <stdexcept>

#include "core/integrators.hpp"

namespace {

using aerial_control::DerivativeFunction;
using aerial_control::StateVector;

using StepFunction = StateVector (*)(
    const DerivativeFunction&, double, const StateVector&, double);

constexpr double kTolerance = 1e-12;

StateVector oscillatorDerivative(const double t, const StateVector& state) {
    static_cast<void>(t);
    StateVector derivative(2);
    derivative << state[1], -state[0];
    return derivative;
}

StateVector constantDerivative(const double t, const StateVector& state) {
    static_cast<void>(t);
    static_cast<void>(state);
    StateVector derivative(2);
    derivative << 2.0, -1.0;
    return derivative;
}

StateVector initialOscillatorState() {
    StateVector state(2);
    state << 1.0, 0.0;
    return state;
}

StateVector simulateOscillator(
    const StepFunction step,
    const double dt,
    const int step_count) {
    const DerivativeFunction derivative = oscillatorDerivative;
    StateVector state = initialOscillatorState();

    for (int index = 0; index < step_count; ++index) {
        const double t = static_cast<double>(index) * dt;
        state = step(derivative, t, state, dt);
    }
    return state;
}

double oscillatorErroeulerStepr(const StateVector& state, const double t) {
    StateVector exact(2);
    exact << std::cos(t), -std::sin(t);
    return (state - exact).norm();
}

TEST(CoreIntegratorTest, EulerIntegratesConstantDerivative) {
    StateVector state(2);
    state << 1.0, 3.0;

    const StateVector result = aerial_control::eulerStep(
        constantDerivative, 0.0, state, 0.1);

    StateVector expected(2);
    expected << 1.2, 2.9;
    EXPECT_TRUE(result.isApprox(expected, kTolerance));
}

TEST(CoreIntegratorTest, Rk4IntegratesConstantDerivative) {
    StateVector state(2);
    state << 1.0, 3.0;

    const StateVector result = aerial_control::rk4Step(
        constantDerivative, 0.0, state, 0.1);

    StateVector expected(2);
    expected << 1.2, 2.9;
    EXPECT_TRUE(result.isApprox(expected, kTolerance));
}

TEST(CoreIntegratorTest, EulerUsesStateDependentDerivative) {
    const StateVector result = aerial_control::eulerStep(
        oscillatorDerivative, 0.0, initialOscillatorState(), 0.1);

    StateVector expected(2);
    expected << 1.0, -0.1;
    EXPECT_TRUE(result.isApprox(expected, kTolerance));
}

TEST(CoreIntegratorTest, Rk4MatchesOscillatorForOneSmallStep) {
    const StateVector result = aerial_control::rk4Step(
        oscillatorDerivative, 0.0, initialOscillatorState(), 0.1);

    StateVector expected(2);
    expected << std::cos(0.1), -std::sin(0.1);
    EXPECT_TRUE(result.isApprox(expected, 1e-7));
}

TEST(CoreIntegratorTest, Rk4ErrorIsSmallerThanEulerAtSameStepSize) {
    constexpr double dt = 0.1;
    constexpr int step_count = 20;
    constexpr double final_time = 2.0;

    const double euler_error = oscillatorError(
        simulateOscillator(aerial_control::eulerStep, dt, step_count),
        final_time);
    const double rk4_error = oscillatorError(
        simulateOscillator(aerial_control::rk4Step, dt, step_count),
        final_time);

    EXPECT_LT(rk4_error, euler_error);
}

TEST(CoreIntegratorTest, EulerErrorFallsWhenStepSizeIsHalved) {
    const double coarse_error = oscillatorError(
        simulateOscillator(aerial_control::eulerStep, 0.2, 10), 2.0);
    const double fine_error = oscillatorError(
        simulateOscillator(aerial_control::eulerStep, 0.1, 20), 2.0);

    EXPECT_LT(fine_error, coarse_error);
}

TEST(CoreIntegratorTest, Rk4ErrorFallsWhenStepSizeIsHalved) {
    const double coarse_error = oscillatorError(
        simulateOscillator(aerial_control::rk4Step, 0.4, 5), 2.0);
    const double fine_error = oscillatorError(
        simulateOscillator(aerial_control::rk4Step, 0.2, 10), 2.0);

    EXPECT_LT(fine_error, coarse_error);
}

TEST(CoreIntegratorTest, Rk4UsesIntermediateTimes) {
    const DerivativeFunction time_dependent = [](
        const double t, const StateVector& state) {
        return StateVector::Constant(state.size(), t);
    };
    const StateVector state = StateVector::Zero(1);

    const StateVector result = aerial_control::rk4Step(
        time_dependent, 0.0, state, 0.2);

    EXPECT_NEAR(result[0], 0.02, kTolerance);
}

TEST(CoreIntegratorTest, DoesNotModifyInputState) {
    const StateVector state = initialOscillatorState();
    const StateVector original = state;

    static_cast<void>(aerial_control::eulerStep(
        oscillatorDerivative, 0.0, state, 0.1));
    static_cast<void>(aerial_control::rk4Step(
        oscillatorDerivative, 0.0, state, 0.1));

    EXPECT_TRUE(state.isApprox(original, 0.0));
}

TEST(CoreIntegratorValidationTest, RejectsNonPositiveStepSize) {
    const StateVector state = initialOscillatorState();

    EXPECT_THROW(
        aerial_control::eulerStep(oscillatorDerivative, 0.0, state, 0.0),
        std::invalid_argument);
    EXPECT_THROW(
        aerial_control::rk4Step(oscillatorDerivative, 0.0, state, -0.1),
        std::invalid_argument);
}

TEST(CoreIntegratorValidationTest, RejectsNonFiniteStepSize) {
    const StateVector state = initialOscillatorState();
    const double nan = std::numeric_limits<double>::quiet_NaN();
    const double infinity = std::numeric_limits<double>::infinity();

    EXPECT_THROW(
        aerial_control::eulerStep(oscillatorDerivative, 0.0, state, nan),
        std::invalid_argument);
    EXPECT_THROW(
        aerial_control::rk4Step(
            oscillatorDerivative, 0.0, state, infinity),
        std::invalid_argument);
}

TEST(CoreIntegratorValidationTest, RejectsEmptyState) {
    const StateVector state;

    EXPECT_THROW(
        aerial_control::eulerStep(oscillatorDerivative, 0.0, state, 0.1),
        std::invalid_argument);
    EXPECT_THROW(
        aerial_control::rk4Step(oscillatorDerivative, 0.0, state, 0.1),
        std::invalid_argument);
}

TEST(CoreIntegratorValidationTest, RejectsNonFiniteState) {
    StateVector state = initialOscillatorState();
    state[1] = std::numeric_limits<double>::quiet_NaN();

    EXPECT_THROW(
        aerial_control::eulerStep(oscillatorDerivative, 0.0, state, 0.1),
        std::invalid_argument);
    EXPECT_THROW(
        aerial_control::rk4Step(oscillatorDerivative, 0.0, state, 0.1),
        std::invalid_argument);
}

TEST(CoreIntegratorValidationTest, EulerRejectsWrongDerivativeDimension) {
    const DerivativeFunction wrong_dimension = [](
        const double t, const StateVector& state) {
        static_cast<void>(t);
        return StateVector::Zero(state.size() + 1);
    };

    EXPECT_THROW(
        aerial_control::eulerStep(
            wrong_dimension, 0.0, initialOscillatorState(), 0.1),
        std::invalid_argument);
}

TEST(CoreIntegratorValidationTest, Rk4ChecksEveryDerivativeDimension) {
    int call_count = 0;
    const DerivativeFunction late_wrong_dimension = [&call_count](
        const double t, const StateVector& state) {
        static_cast<void>(t);
        ++call_count;
        if (call_count == 3) {
            return StateVector::Zero(state.size() + 1);
        }
        return StateVector::Zero(state.size());
    };

    EXPECT_THROW(
        aerial_control::rk4Step(
            late_wrong_dimension, 0.0, initialOscillatorState(), 0.1),
        std::invalid_argument);
    EXPECT_EQ(call_count, 3);
}

TEST(CoreIntegratorValidationTest, EulerRejectsNonFiniteDerivative) {
    const DerivativeFunction non_finite = [](
        const double t, const StateVector& state) {
        static_cast<void>(t);
        return StateVector::Constant(
            state.size(), std::numeric_limits<double>::infinity());
    };

    EXPECT_THROW(
        aerial_control::eulerStep(
            non_finite, 0.0, initialOscillatorState(), 0.1),
        std::invalid_argument);
}

TEST(CoreIntegratorValidationTest, Rk4ChecksEveryDerivativeForFiniteValues) {
    int call_count = 0;
    const DerivativeFunction late_non_finite = [&call_count](
        const double t, const StateVector& state) {
        static_cast<void>(t);
        ++call_count;
        StateVector result = StateVector::Zero(state.size());
        if (call_count == 4) {
            result[0] = std::numeric_limits<double>::quiet_NaN();
        }
        return result;
    };

    EXPECT_THROW(
        aerial_control::rk4Step(
            late_non_finite, 0.0, initialOscillatorState(), 0.1),
        std::invalid_argument);
    EXPECT_EQ(call_count, 4);
}

}  // namespace
