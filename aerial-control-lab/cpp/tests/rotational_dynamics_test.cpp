#include "core/rotational_dynamics.hpp"

#include <gtest/gtest.h>

#include <array>
#include <limits>
#include <stdexcept>

namespace aerial_control
{
  namespace
  {

    const RotationalParameters kDefaultParameters{Eigen::Vector3d{0.02, 0.03, 0.04}};
    const RotationalInput kZeroMoment{Eigen::Vector3d::Zero()};

    TEST(RotationalDynamicsTest, ZeroRateAndZeroMomentGiveZeroAcceleration)
    {
      const StateVector acceleration = angularRateDerivative(0.0, StateVector::Zero(3), kDefaultParameters, kZeroMoment);

      ASSERT_EQ(acceleration.size(), 3);
      EXPECT_TRUE(acceleration.isZero(0.0));
    }

    TEST(RotationalDynamicsTest, PrincipalAxisRotationHasNoTorqueFreeCoupling)
    {
      for (Eigen::Index axis = 0; axis < 3; ++axis)
      {
        SCOPED_TRACE(axis);
        StateVector state = StateVector::Zero(3);
        state(axis) = -2.0;

        EXPECT_TRUE(angularRateDerivative(0.0, state, kDefaultParameters, kZeroMoment).isZero(0.0));
      }
    }

    TEST(RotationalDynamicsTest, UnequalInertiaMatchesCheckedMixedRateReference)
    {
      StateVector state(3);
      state << 1.0, 2.0, 3.0;

      const StateVector acceleration = angularRateDerivative(0.0, state, kDefaultParameters, kZeroMoment);

      ASSERT_EQ(acceleration.size(), 3);
      EXPECT_NEAR(acceleration(0), -3.0, 1e-12);
      EXPECT_NEAR(acceleration(1), 2.0, 1e-12);
      EXPECT_NEAR(acceleration(2), -0.5, 1e-12);
    }

    TEST(RotationalDynamicsTest, EqualInertiaHasZeroTorqueFreeAcceleration)
    {
      StateVector state(3);
      state << 1.0, 2.0, 3.0;
      const RotationalParameters parameters{Eigen::Vector3d::Constant(0.03)};

      const StateVector acceleration = angularRateDerivative(0.0, state, parameters, kZeroMoment);

      ASSERT_EQ(acceleration.size(), 3);
      EXPECT_NEAR(acceleration(0), 0.0, 1e-12);
      EXPECT_NEAR(acceleration(1), 0.0, 1e-12);
      EXPECT_NEAR(acceleration(2), 0.0, 1e-12);
    }

    TEST(RotationalDynamicsTest, AppliesExternalMomentWithBothSigns)
    {
      StateVector state(3);
      state << 1.0, 2.0, 3.0;
      const RotationalInput input{Eigen::Vector3d{0.02, -0.03, 0.08}};

      const StateVector acceleration = angularRateDerivative(-1.0, state, kDefaultParameters, input);

      EXPECT_NEAR(acceleration(0), -2.0, 1e-12);
      EXPECT_NEAR(acceleration(1), 1.0, 1e-12);
      EXPECT_NEAR(acceleration(2), 1.5, 1e-12);
    }

    TEST(RotationalDynamicsTest, DoesNotModifyStateParametersOrInput)
    {
      StateVector state(3);
      state << 1.0, 2.0, 3.0;
      RotationalParameters parameters = kDefaultParameters;
      RotationalInput input{Eigen::Vector3d{0.01, -0.02, 0.03}};
      const StateVector originalState = state;
      const Eigen::Vector3d originalInertia = parameters.principalInertiaKgM2;
      const Eigen::Vector3d originalMoment = input.momentBodyNm;

      static_cast<void>(angularRateDerivative(0.0, state, parameters, input));

      EXPECT_TRUE((state.array() == originalState.array()).all());
      EXPECT_TRUE((parameters.principalInertiaKgM2.array() == originalInertia.array()).all());
      EXPECT_TRUE((input.momentBodyNm.array() == originalMoment.array()).all());
    }

    TEST(RotationalDynamicsTest, RejectsIncorrectStateDimensions)
    {
      for (const Eigen::Index size : std::array<Eigen::Index, 4>{0, 1, 2, 4})
      {
        SCOPED_TRACE(size);
        EXPECT_THROW(angularRateDerivative(0.0, StateVector::Zero(size), kDefaultParameters, kZeroMoment), std::invalid_argument);
      }
    }

    TEST(RotationalDynamicsTest, RejectsNonfiniteTimeStateAndMoment)
    {
      const std::array<double, 3> invalidValues{std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::infinity(), -std::numeric_limits<double>::infinity()};
      for (const double invalidValue : invalidValues)
      {
        SCOPED_TRACE(invalidValue);
        EXPECT_THROW(angularRateDerivative(invalidValue, StateVector::Zero(3), kDefaultParameters, kZeroMoment), std::invalid_argument);
        for (Eigen::Index axis = 0; axis < 3; ++axis)
        {
          SCOPED_TRACE(axis);
          StateVector state = StateVector::Zero(3);
          state(axis) = invalidValue;
          EXPECT_THROW(angularRateDerivative(0.0, state, kDefaultParameters, kZeroMoment), std::invalid_argument);

          RotationalInput input = kZeroMoment;
          input.momentBodyNm(axis) = invalidValue;
          EXPECT_THROW(angularRateDerivative(0.0, StateVector::Zero(3), kDefaultParameters, input), std::invalid_argument);
        }
      }
    }

    TEST(RotationalDynamicsTest, RejectsNonpositiveAndNonfinitePrincipalInertias)
    {
      const std::array<double, 5> invalidValues{0.0, -0.01, std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::infinity(), -std::numeric_limits<double>::infinity()};
      for (const double invalidValue : invalidValues)
      {
        SCOPED_TRACE(invalidValue);
        for (Eigen::Index axis = 0; axis < 3; ++axis)
        {
          SCOPED_TRACE(axis);
          RotationalParameters parameters = kDefaultParameters;
          parameters.principalInertiaKgM2(axis) = invalidValue;
          EXPECT_THROW(angularRateDerivative(0.0, StateVector::Zero(3), parameters, kZeroMoment), std::invalid_argument);
        }
      }
    }

    TEST(RotationalDynamicsTest, RejectsNonfiniteResultFromFiniteInputs)
    {
      const double largest = std::numeric_limits<double>::max();
      const RotationalInput input{Eigen::Vector3d{largest, 0.0, 0.0}};

      EXPECT_THROW(angularRateDerivative(0.0, StateVector::Zero(3), kDefaultParameters, input), std::invalid_argument);
    }

  } // namespace
} // namespace aerial_control
