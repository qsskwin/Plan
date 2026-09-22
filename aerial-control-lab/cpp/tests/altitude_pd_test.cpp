#include "core/altitude_pd.hpp"

#include <gtest/gtest.h>

#include <limits>
#include <stdexcept>

namespace aerial_control
{
  namespace
  {

    AltitudePdParameters makeDefaultParameters()
    {
      return AltitudePdParameters{
          1.5,      // nominalMassKg
          4.0,      // kpPerSecondSquared
          4.0,      // kdPerSecond
          0.0,      // minThrustN
          29.41995, // maxThrustN
      };
    }

    TEST(AltitudePdControllerTest, ZeroErrorAndZeroSpeedProducesHoverThrust)
    {
      const AltitudePdController controller{makeDefaultParameters()};

      const AltitudeCommand command = controller.compute(
          /* referenceHeightM = */ 1.0,
          /* heightM = */ 1.0,
          /* verticalSpeedMps = */ 0.0);

      EXPECT_NEAR(command.rawThrustN, 14.709975, 1e-12);

      EXPECT_NEAR(command.appliedThrustN, 14.709975, 1e-12);
    }

    TEST(AltitudePdControllerTest, BelowReferenceIncreasesThrust)
    {
      const AltitudePdController controller{makeDefaultParameters()};

      const AltitudeCommand command = controller.compute(
          /* referenceHeightM = */ 1.0,
          /* heightM = */ 0.0,
          /* verticalSpeedMps = */ 0.0);

      EXPECT_NEAR(command.rawThrustN, 20.709975, 1e-12);

      EXPECT_NEAR(command.appliedThrustN, 20.709975, 1e-12);
    }

    TEST(AltitudePdControllerTest, UpwardSpeedReducesThrust)
    {
      const AltitudePdController controller{makeDefaultParameters()};

      const AltitudeCommand command = controller.compute(
          /* referenceHeightM = */ 1.0,
          /* heightM = */ 1.0,
          /* verticalSpeedMps = */ 0.5);

      EXPECT_NEAR(command.rawThrustN, 11.709975, 1e-12);
      EXPECT_NEAR(command.appliedThrustN, 11.709975, 1e-12);
      EXPECT_LT(command.appliedThrustN, 14.709975);
    }

    TEST(AltitudePdControllerTest, ClampsThrustToUpperAndLowerLimits)
    {
      const AltitudePdController controller{makeDefaultParameters()};

      const AltitudeCommand upperCommand = controller.compute(
          /* referenceHeightM = */ 10.0,
          /* heightM = */ 0.0,
          /* verticalSpeedMps = */ 0.0);
      const AltitudeCommand lowerCommand = controller.compute(
          /* referenceHeightM = */ 0.0,
          /* heightM = */ 10.0,
          /* verticalSpeedMps = */ 0.0);

      EXPECT_NEAR(upperCommand.rawThrustN, 74.709975, 1e-12);
      EXPECT_NEAR(upperCommand.appliedThrustN, 29.41995, 1e-12);
      EXPECT_NEAR(lowerCommand.rawThrustN, -45.290025, 1e-12);
      EXPECT_NEAR(lowerCommand.appliedThrustN, 0.0, 1e-12);
    }

    TEST(AltitudePdControllerTest, RejectsOutOfRangeParameters)
    {
      AltitudePdParameters parameters = makeDefaultParameters();

      parameters.nominalMassKg = 0.0;
      EXPECT_THROW(static_cast<void>(AltitudePdController{parameters}), std::invalid_argument);

      parameters = makeDefaultParameters();
      parameters.kpPerSecondSquared = -1.0;
      EXPECT_THROW(static_cast<void>(AltitudePdController{parameters}), std::invalid_argument);

      parameters = makeDefaultParameters();
      parameters.kdPerSecond = -1.0;
      EXPECT_THROW(static_cast<void>(AltitudePdController{parameters}), std::invalid_argument);

      parameters = makeDefaultParameters();
      parameters.minThrustN = -1.0;
      EXPECT_THROW(static_cast<void>(AltitudePdController{parameters}), std::invalid_argument);

      parameters = makeDefaultParameters();
      parameters.maxThrustN = parameters.minThrustN - 1.0;
      EXPECT_THROW(static_cast<void>(AltitudePdController{parameters}), std::invalid_argument);
    }

    TEST(AltitudePdControllerTest, RejectsNonFiniteParameters)
    {
      const double nan = std::numeric_limits<double>::quiet_NaN();
      const double infinity = std::numeric_limits<double>::infinity();
      AltitudePdParameters parameters = makeDefaultParameters();

      parameters.nominalMassKg = nan;
      EXPECT_THROW(static_cast<void>(AltitudePdController{parameters}), std::invalid_argument);

      parameters = makeDefaultParameters();
      parameters.kpPerSecondSquared = infinity;
      EXPECT_THROW(static_cast<void>(AltitudePdController{parameters}), std::invalid_argument);

      parameters = makeDefaultParameters();
      parameters.kdPerSecond = nan;
      EXPECT_THROW(static_cast<void>(AltitudePdController{parameters}), std::invalid_argument);

      parameters = makeDefaultParameters();
      parameters.minThrustN = infinity;
      EXPECT_THROW(static_cast<void>(AltitudePdController{parameters}), std::invalid_argument);

      parameters = makeDefaultParameters();
      parameters.maxThrustN = infinity;
      EXPECT_THROW(static_cast<void>(AltitudePdController{parameters}), std::invalid_argument);
    }

    TEST(AltitudePdControllerTest, RejectsNonFiniteComputeInputs)
    {
      const AltitudePdController controller{makeDefaultParameters()};
      const double nan = std::numeric_limits<double>::quiet_NaN();
      const double infinity = std::numeric_limits<double>::infinity();

      EXPECT_THROW(controller.compute(nan, 1.0, 0.0), std::invalid_argument);
      EXPECT_THROW(controller.compute(1.0, nan, 0.0), std::invalid_argument);
      EXPECT_THROW(controller.compute(1.0, 1.0, infinity), std::invalid_argument);
    }

  } // namespace
} // namespace aerial_control
