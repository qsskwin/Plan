#include "core/point_mass.hpp"

#include <Eigen/Core>
#include <Eigen/Geometry>
#include <gtest/gtest.h>

#include <array>
#include <cmath>
#include <limits>
#include <stdexcept>

namespace {

using aerial_control::DerivativeFunction;
using aerial_control::PointMassInput;
using aerial_control::PointMassParameters;
using aerial_control::StateVector;
using aerial_control::pointMassDerivative;
using aerial_control::rk4Step;

constexpr double kDerivativeTolerance = 1e-12;
constexpr double kTrajectoryTolerance = 1e-10;

void expectStateNear(
    const StateVector& actual,
    const StateVector& expected,
    const double tolerance) {
  ASSERT_EQ(actual.size(), expected.size());
  for (Eigen::Index index = 0; index < actual.size(); ++index) {
    EXPECT_NEAR(actual(index), expected(index), tolerance);
  }
}

TEST(PointMassDerivativeTest, PositionDerivativeEqualsVelocity) {
  StateVector state(6);
  state << 1.0, 2.0, 3.0, 4.0, 5.0, 6.0;  // Position (1,2,3), Velocity (4,5,6)
  PointMassParameters parameters{1.0};    // Mass = 1 kg
  const StateVector derivative =
      pointMassDerivative(0.0, state, parameters,
                          PointMassInput{0.0, Eigen::Quaterniond::Identity()});

  ASSERT_EQ(derivative.size(), 6);
  EXPECT_DOUBLE_EQ(derivative(0), state(3));
  EXPECT_DOUBLE_EQ(derivative(1), state(4));
  EXPECT_DOUBLE_EQ(derivative(2), state(5));
}

TEST(PointMassDerivativeTest, ZeroThrustProducesNedGravity) {
  StateVector state(6);
  state << 0.0, 0.0, 0.0, 0.0, 0.0, 0.0;  // Position (0,0,0), Velocity (0,0,0)
  PointMassParameters parameters{1.5};    // Mass = 1.5 kg
  const StateVector derivative =
      pointMassDerivative(0.0, state, parameters,
                          PointMassInput{0.0, Eigen::Quaterniond::Identity()});

  ASSERT_EQ(derivative.size(), 6);
  EXPECT_DOUBLE_EQ(derivative(3), 0.0);
  EXPECT_DOUBLE_EQ(derivative(4), 0.0);
  EXPECT_DOUBLE_EQ(derivative(5), aerial_control::kStandardGravityMps2);
}

TEST(PointMassDerivativeTest, HoverThrustCancelsNedGravity) {
  StateVector state(6);
  state << 0.0, 0.0, 0.0, 0.0, 0.0, 0.0;  // Position (0,0,0), Velocity (0,0,0)
  PointMassParameters parameters{1.5};    // Mass = 1.5 kg
  const double hover_thrust_n =
      parameters.massKg * aerial_control::kStandardGravityMps2;
  const StateVector derivative = pointMassDerivative(
      0.0, state, parameters,
      PointMassInput{hover_thrust_n, Eigen::Quaterniond::Identity()});

  ASSERT_EQ(derivative.size(), 6);
  EXPECT_NEAR(derivative(3), 0.0, 1e-12);
  EXPECT_NEAR(derivative(4), 0.0, 1e-12);
  EXPECT_NEAR(derivative(5), 0.0, 1e-12);
}

TEST(PointMassDerivativeTest, PositiveRollProducesExpectedEastAcceleration) {
  StateVector state(6);
  state << 0.0, 0.0, 0.0, 0.0, 0.0, 0.0;  // Position (0,0,0), Velocity (0,0,0)
  const double phi_rad = 10.0 * std::acos(-1.0) / 180.0;
  PointMassParameters parameters{1.5};  // Mass = 1.5 kg
  const double compensated_thrust_n =
      parameters.massKg * aerial_control::kStandardGravityMps2 / std::cos(phi_rad);

  const Eigen::Quaterniond q_nb{
      std::cos(phi_rad / 2.0),
      std::sin(phi_rad / 2.0),
      0.0,
      0.0,
  };
  const StateVector derivative = pointMassDerivative(
      0.0, state, parameters, PointMassInput{compensated_thrust_n, q_nb});

  ASSERT_EQ(derivative.size(), 6);
  EXPECT_NEAR(derivative(3), 0.0, 1e-12);
  EXPECT_NEAR(derivative(4),
              std::tan(phi_rad) * aerial_control::kStandardGravityMps2, 1e-12);
  EXPECT_NEAR(derivative(5), 0.0, 1e-12);
}

TEST(PointMassDerivativeTest, ZeroMassIsRejected) {
  const StateVector state = StateVector::Zero(6);
  const PointMassParameters parameters{0.0};
  const PointMassInput input{
      0.0,
      Eigen::Quaterniond::Identity(),
  };

  EXPECT_THROW(
      pointMassDerivative(0.0, state, parameters, input),
      std::invalid_argument);
}

TEST(PointMassDerivativeTest, NegativeThrustIsRejected) {
  const StateVector state = StateVector::Zero(6);
  const PointMassParameters parameters{1.5};
  const PointMassInput input{
      -1.0,
      Eigen::Quaterniond::Identity(),
  };

  EXPECT_THROW(
      pointMassDerivative(0.0, state, parameters, input),
      std::invalid_argument);
}

TEST(PointMassDerivativeTest, InvalidStateDimensionIsRejected) {
  const StateVector state = StateVector::Zero(5);
  const PointMassParameters parameters{1.5};
  const PointMassInput input{
      0.0,
      Eigen::Quaterniond::Identity(),
  };

  EXPECT_THROW(
      pointMassDerivative(0.0, state, parameters, input),
      std::invalid_argument);
}

TEST(PointMassDerivativeTest, DoesNotModifyInputState) {
  StateVector state(6);
  state << 1.0, -2.0, 3.0, -4.0, 5.0, -6.0;
  const StateVector original = state;
  const PointMassParameters parameters{1.5};
  const PointMassInput input{
      2.0,
      Eigen::Quaterniond::Identity(),
  };

  static_cast<void>(
      pointMassDerivative(0.0, state, parameters, input));

  EXPECT_TRUE(state.isApprox(original, 0.0));
}

TEST(PointMassDerivativeTest, NegativeAndNonfiniteMassesAreRejected) {
  const StateVector state = StateVector::Zero(6);
  const PointMassInput input{
      0.0,
      Eigen::Quaterniond::Identity(),
  };
  const double infinity = std::numeric_limits<double>::infinity();
  const std::array<double, 4> invalid_masses{
      -1.0,
      std::numeric_limits<double>::quiet_NaN(),
      infinity,
      -infinity,
  };

  for (const double invalid_mass : invalid_masses) {
    SCOPED_TRACE(
        ::testing::Message() << "invalid_mass=" << invalid_mass);
    const PointMassParameters parameters{invalid_mass};
    EXPECT_THROW(
        pointMassDerivative(0.0, state, parameters, input),
        std::invalid_argument);
  }
}

TEST(PointMassDerivativeTest, NonfiniteThrustsAreRejected) {
  const StateVector state = StateVector::Zero(6);
  const PointMassParameters parameters{1.5};
  const double infinity = std::numeric_limits<double>::infinity();
  const std::array<double, 3> invalid_thrusts{
      std::numeric_limits<double>::quiet_NaN(),
      infinity,
      -infinity,
  };

  for (const double invalid_thrust : invalid_thrusts) {
    SCOPED_TRACE(
        ::testing::Message() << "invalid_thrust=" << invalid_thrust);
    const PointMassInput input{
        invalid_thrust,
        Eigen::Quaterniond::Identity(),
    };
    EXPECT_THROW(
        pointMassDerivative(0.0, state, parameters, input),
        std::invalid_argument);
  }
}

TEST(PointMassDerivativeTest, NonfiniteStatesAreRejected) {
  const PointMassParameters parameters{1.5};
  const PointMassInput input{
      0.0,
      Eigen::Quaterniond::Identity(),
  };
  const double infinity = std::numeric_limits<double>::infinity();
  const std::array<double, 3> invalid_values{
      std::numeric_limits<double>::quiet_NaN(),
      infinity,
      -infinity,
  };

  for (const double invalid_value : invalid_values) {
    SCOPED_TRACE(
        ::testing::Message() << "invalid_state_value=" << invalid_value);
    StateVector state = StateVector::Zero(6);
    state(2) = invalid_value;
    EXPECT_THROW(
        pointMassDerivative(0.0, state, parameters, input),
        std::invalid_argument);
  }
}

TEST(PointMassDerivativeTest, InvalidQuaternionsAreRejected) {
  const StateVector state = StateVector::Zero(6);
  const PointMassParameters parameters{1.5};
  const double infinity = std::numeric_limits<double>::infinity();
  const double quiet_nan = std::numeric_limits<double>::quiet_NaN();
  const std::array<Eigen::Quaterniond, 4> invalid_quaternions{
      Eigen::Quaterniond{0.0, 0.0, 0.0, 0.0},
      Eigen::Quaterniond{quiet_nan, 0.0, 0.0, 1.0},
      Eigen::Quaterniond{1.0, infinity, 0.0, 0.0},
      Eigen::Quaterniond{1.0, 0.0, -infinity, 0.0},
  };

  for (std::size_t index = 0; index < invalid_quaternions.size(); ++index) {
    SCOPED_TRACE(::testing::Message() << "quaternion_index=" << index);
    const PointMassInput input{
        0.0,
        invalid_quaternions[index],
    };
    EXPECT_THROW(
        pointMassDerivative(0.0, state, parameters, input),
        std::invalid_argument);
  }
}

TEST(PointMassDerivativeTest, NonunitQuaternionIsNormalized) {
  StateVector state(6);
  state << 1.0, -2.0, 3.0, -4.0, 5.0, -6.0;
  const PointMassParameters parameters{1.5};
  const double phi_rad = 10.0 * std::acos(-1.0) / 180.0;
  const Eigen::Quaterniond unit_q_nb{
      std::cos(phi_rad / 2.0),
      std::sin(phi_rad / 2.0),
      0.0,
      0.0,
  };
  const Eigen::Quaterniond scaled_q_nb{
      3.0 * unit_q_nb.w(),
      3.0 * unit_q_nb.x(),
      3.0 * unit_q_nb.y(),
      3.0 * unit_q_nb.z(),
  };
  const PointMassInput unit_input{4.0, unit_q_nb};
  const PointMassInput scaled_input{4.0, scaled_q_nb};

  const StateVector expected =
      pointMassDerivative(0.0, state, parameters, unit_input);
  const StateVector actual =
      pointMassDerivative(0.0, state, parameters, scaled_input);

  expectStateNear(actual, expected, kDerivativeTolerance);
}

TEST(PointMassDerivativeTest, FreeFallTrajectoryMatchesAnalyticSolution) {
  const PointMassParameters parameters{1.5};
  const PointMassInput input{
      0.0,
      Eigen::Quaterniond::Identity(),
  };
  const DerivativeFunction dynamics =
      [&parameters, &input](const double t, const StateVector& state) {
        return pointMassDerivative(t, state, parameters, input);
      };
  constexpr double dt = 0.01;
  constexpr int step_count = 200;
  StateVector state = StateVector::Zero(6);
  int checked_state_count = 0;

  for (int step = 0; step <= step_count; ++step) {
    SCOPED_TRACE(::testing::Message() << "step=" << step);
    const double t = static_cast<double>(step) * dt;
    StateVector expected = StateVector::Zero(6);
    expected(2) =
        0.5 * aerial_control::kStandardGravityMps2 * t * t;
    expected(5) = aerial_control::kStandardGravityMps2 * t;

    EXPECT_TRUE(state.allFinite());
    expectStateNear(state, expected, kTrajectoryTolerance);
    ++checked_state_count;

    if (step < step_count) {
      state = rk4Step(dynamics, t, state, dt);
    }
  }

  EXPECT_EQ(checked_state_count, step_count + 1);
}

TEST(PointMassDerivativeTest, HoverTrajectoryRemainsStationary) {
  const PointMassParameters parameters{1.5};
  const double hover_thrust_n =
      parameters.massKg * aerial_control::kStandardGravityMps2;
  const PointMassInput input{
      hover_thrust_n,
      Eigen::Quaterniond::Identity(),
  };
  const DerivativeFunction dynamics =
      [&parameters, &input](const double t, const StateVector& state) {
        return pointMassDerivative(t, state, parameters, input);
      };
  constexpr double dt = 0.01;
  constexpr int step_count = 1000;
  const StateVector expected = StateVector::Zero(6);
  StateVector state = StateVector::Zero(6);
  int checked_state_count = 0;

  for (int step = 0; step <= step_count; ++step) {
    SCOPED_TRACE(::testing::Message() << "step=" << step);
    const double t = static_cast<double>(step) * dt;

    EXPECT_TRUE(state.allFinite());
    expectStateNear(state, expected, kTrajectoryTolerance);
    ++checked_state_count;

    if (step < step_count) {
      state = rk4Step(dynamics, t, state, dt);
    }
  }

  EXPECT_EQ(checked_state_count, step_count + 1);
}

TEST(PointMassDerivativeTest, PositiveRollTrajectoryMatchesAnalyticSolution) {
  const PointMassParameters parameters{1.5};
  const double phi_rad = 10.0 * std::acos(-1.0) / 180.0;
  const Eigen::Quaterniond q_nb{
      std::cos(phi_rad / 2.0),
      std::sin(phi_rad / 2.0),
      0.0,
      0.0,
  };
  const double compensated_thrust_n =
      parameters.massKg * aerial_control::kStandardGravityMps2 /
      std::cos(phi_rad);
  const PointMassInput input{
      compensated_thrust_n,
      q_nb,
  };
  const DerivativeFunction dynamics =
      [&parameters, &input](const double t, const StateVector& state) {
        return pointMassDerivative(t, state, parameters, input);
      };
  const double east_acceleration =
      aerial_control::kStandardGravityMps2 * std::tan(phi_rad);
  constexpr double dt = 0.01;
  constexpr int step_count = 200;
  StateVector state = StateVector::Zero(6);
  int checked_state_count = 0;

  for (int step = 0; step <= step_count; ++step) {
    SCOPED_TRACE(::testing::Message() << "step=" << step);
    const double t = static_cast<double>(step) * dt;
    StateVector expected = StateVector::Zero(6);
    expected(1) = 0.5 * east_acceleration * t * t;
    expected(4) = east_acceleration * t;

    EXPECT_TRUE(state.allFinite());
    expectStateNear(state, expected, kTrajectoryTolerance);
    ++checked_state_count;

    if (step < step_count) {
      state = rk4Step(dynamics, t, state, dt);
    }
  }

  EXPECT_EQ(checked_state_count, step_count + 1);
}

}  // namespace
