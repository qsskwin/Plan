#include "core/attitude_kinematics.hpp"

#include <gtest/gtest.h>

#include <cmath>
#include <limits>
#include <stdexcept>

namespace aerial_control
{
  namespace
  {

    TEST(AttitudeKinematicsTest, ZeroAngularRateGivesZeroDerivative)
    {
      StateVector q(4);
      q << 1.0, 0.0, 0.0, 0.0;

      const StateVector qDot = attitudeKinematicsDerivative(0.0, q, Eigen::Vector3d::Zero());

      ASSERT_EQ(qDot.size(), 4);
      EXPECT_TRUE(qDot.isZero(0.0));
    }

    TEST(AttitudeKinematicsTest, IdentityAttitudeTracksAngularRateSign)
    {
      StateVector q(4);
      q << 1.0, 0.0, 0.0, 0.0;

      const StateVector positive = attitudeKinematicsDerivative(0.0, q, Eigen::Vector3d{0.0, 0.0, 2.0});
      const StateVector negative = attitudeKinematicsDerivative(0.0, q, Eigen::Vector3d{0.0, 0.0, -2.0});

      EXPECT_DOUBLE_EQ(positive(0), 0.0);
      EXPECT_DOUBLE_EQ(positive(1), 0.0);
      EXPECT_DOUBLE_EQ(positive(2), 0.0);
      EXPECT_DOUBLE_EQ(positive(3), 1.0);
      EXPECT_DOUBLE_EQ(negative(3), -1.0);
    }

    TEST(AttitudeKinematicsTest, NonidentityAttitudePreservesBodyRateOrder)
    {
      const double halfSqrtTwo = std::sqrt(0.5);
      StateVector q(4);
      q << halfSqrtTwo, 0.0, 0.0, halfSqrtTwo;

      const StateVector qDot = attitudeKinematicsDerivative(0.0, q, Eigen::Vector3d{2.0, 0.0, 0.0});

      EXPECT_NEAR(qDot(0), 0.0, 1e-15);
      EXPECT_NEAR(qDot(1), halfSqrtTwo, 1e-15);
      EXPECT_NEAR(qDot(2), halfSqrtTwo, 1e-15);
      EXPECT_NEAR(qDot(3), 0.0, 1e-15);
    }

    TEST(AttitudeKinematicsTest, NonunitIntermediateStateIsNotNormalized)
    {
      StateVector q(4);
      q << 2.0, 1.0, 0.0, 0.0;

      const StateVector qDot = attitudeKinematicsDerivative(0.0, q, Eigen::Vector3d{1.0, 2.0, 3.0});

      EXPECT_DOUBLE_EQ(qDot(0), -0.5);
      EXPECT_DOUBLE_EQ(qDot(1), 1.0);
      EXPECT_DOUBLE_EQ(qDot(2), 0.5);
      EXPECT_DOUBLE_EQ(qDot(3), 4.0);
      EXPECT_DOUBLE_EQ(q(0), 2.0);
      EXPECT_DOUBLE_EQ(q(1), 1.0);
    }

    TEST(AttitudeKinematicsTest, RejectsInvalidInputs)
    {
      StateVector q(4);
      q << 1.0, 0.0, 0.0, 0.0;
      const Eigen::Vector3d zeroRate = Eigen::Vector3d::Zero();

      EXPECT_THROW(attitudeKinematicsDerivative(0.0, StateVector::Zero(3), zeroRate), std::invalid_argument);
      EXPECT_THROW(attitudeKinematicsDerivative(0.0, StateVector::Zero(4), zeroRate), std::invalid_argument);

      q(1) = std::numeric_limits<double>::quiet_NaN();
      EXPECT_THROW(attitudeKinematicsDerivative(0.0, q, zeroRate), std::invalid_argument);
      q(1) = 0.0;

      EXPECT_THROW(attitudeKinematicsDerivative(0.0, q, Eigen::Vector3d{std::numeric_limits<double>::infinity(), 0.0, 0.0}), std::invalid_argument);
      EXPECT_THROW(attitudeKinematicsDerivative(std::numeric_limits<double>::quiet_NaN(), q, zeroRate), std::invalid_argument);
    }

  } // namespace
} // namespace aerial_control
