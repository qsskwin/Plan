#include "core/attitude_rate_dynamics.hpp"

#include <gtest/gtest.h>

namespace aerial_control
{
  namespace
  {

    TEST(AttitudeRateDynamicsTest, ZeroRateAndZeroMomentGiveZeroDerivative)
    {
      StateVector state(7);
      state << 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0;
      const RotationalParameters parameters{Eigen::Vector3d{0.02, 0.03, 0.04}};
      const RotationalInput input{Eigen::Vector3d::Zero()};

      const StateVector derivative = attitudeRateDerivative(0.0, state, parameters, input);

      ASSERT_EQ(derivative.size(), 7);
      EXPECT_TRUE(derivative.isZero(0.0));
    }

    TEST(AttitudeRateDynamicsTest, MixedAngularRateGivesExpectedDerivative)
    {
      StateVector state(7);
      state << 1.0, 0.0, 0.0, 0.0, 1.0, 2.0, 3.0;
      const RotationalParameters parameters{Eigen::Vector3d{0.02, 0.03, 0.04}};
      const RotationalInput input{Eigen::Vector3d::Zero()};
      StateVector expectedDerivative(7);
      expectedDerivative << 0.0, 0.5, 1.0, 1.5, -3.0, 2.0, -0.5;

      const StateVector derivative = attitudeRateDerivative(0.0, state, parameters, input);

      ASSERT_EQ(derivative.size(), 7);
      EXPECT_TRUE(derivative.isApprox(expectedDerivative, 1e-12));
    }

    TEST(AttitudeRateDynamicsTest, DoublingQuaternionDoublesOnlyQuaternionDerivative)
    {
      StateVector state(7);
      state << 1.0, 0.0, 0.0, 0.0, 1.0, 2.0, 3.0;
      StateVector doubledQuaternionState = state;
      doubledQuaternionState.head<4>() *= 2.0;
      const RotationalParameters parameters{Eigen::Vector3d{0.02, 0.03, 0.04}};
      const RotationalInput input{Eigen::Vector3d::Zero()};

      const StateVector derivative = attitudeRateDerivative(0.0, state, parameters, input);
      const StateVector doubledQuaternionDerivative =
          attitudeRateDerivative(0.0, doubledQuaternionState, parameters, input);

      ASSERT_EQ(derivative.size(), 7);
      ASSERT_EQ(doubledQuaternionDerivative.size(), 7);
      EXPECT_TRUE(doubledQuaternionDerivative.head<4>().isApprox(2.0 * derivative.head<4>(), 1e-12));
      EXPECT_TRUE(doubledQuaternionDerivative.tail<3>().isApprox(derivative.tail<3>(), 1e-12));
    }

  } // namespace
} // namespace aerial_control
