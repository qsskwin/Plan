#include "control_utils.hpp"

#include <limits>
#include <stdexcept>

#include <gtest/gtest.h>

TEST(ClampValueTest, ReturnsValueWithinBounds) {
  EXPECT_DOUBLE_EQ(
      0.5,
      aerial_control::week2_mon::ClampValue(0.5, 0.0, 1.0));
}

TEST(ClampValueTest, ReturnsLowerBoundAtLowerBound) {
  EXPECT_DOUBLE_EQ(
      -1.0,
      aerial_control::week2_mon::ClampValue(-1.0, -1.0, 2.0));
}

TEST(ClampValueTest, ReturnsUpperBoundAtUpperBound) {
  EXPECT_DOUBLE_EQ(
      2.0,
      aerial_control::week2_mon::ClampValue(2.0, -1.0, 2.0));
}

TEST(ClampValueTest, ClampsValueBelowLowerBound) {
  EXPECT_DOUBLE_EQ(
      -1.0,
      aerial_control::week2_mon::ClampValue(-3.0, -1.0, 2.0));
}

TEST(ClampValueTest, ClampsValueAboveUpperBound) {
  EXPECT_DOUBLE_EQ(
      2.0,
      aerial_control::week2_mon::ClampValue(4.0, -1.0, 2.0));
}

TEST(ClampValueTest, ThrowsForReversedBounds) {
  EXPECT_THROW(
      aerial_control::week2_mon::ClampValue(0.0, 2.0, -1.0),
      std::invalid_argument);
}

TEST(IsFiniteVectorTest, ReturnsTrueForFiniteVector) {
  const Eigen::Vector3d vector(1.0, -2.0, 0.0);

  EXPECT_TRUE(
      aerial_control::week2_mon::IsFiniteVector(vector));
}

TEST(IsFiniteVectorTest, ReturnsFalseForNanComponent) {
  const double nan = std::numeric_limits<double>::quiet_NaN();
  const Eigen::Vector3d vector(1.0, nan, 3.0);

  EXPECT_FALSE(
      aerial_control::week2_mon::IsFiniteVector(vector));
}

TEST(IsFiniteVectorTest, ReturnsFalseForPositiveInfinity) {
  const double infinity = std::numeric_limits<double>::infinity();
  const Eigen::Vector3d vector(1.0, infinity, 3.0);

  EXPECT_FALSE(
      aerial_control::week2_mon::IsFiniteVector(vector));
}

TEST(IsFiniteVectorTest, ReturnsFalseForNegativeInfinity) {
  const double negative_infinity =
      -std::numeric_limits<double>::infinity();
  const Eigen::Vector3d vector(negative_infinity, 2.0, 3.0);

  EXPECT_FALSE(
      aerial_control::week2_mon::IsFiniteVector(vector));
}
