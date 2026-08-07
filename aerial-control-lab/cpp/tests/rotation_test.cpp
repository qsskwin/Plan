#include <Eigen/Core>
#include <Eigen/Geometry>
#include <gtest/gtest.h>

#include <cmath>
#include <limits>
#include <stdexcept>

#include "core/rotation.hpp"

namespace {

constexpr double kTolerance = 1e-12;

Eigen::Quaterniond axisAngleQuaternion(
    const Eigen::Vector3d& axis,
    const double angle_rad) {
    return Eigen::Quaterniond{Eigen::AngleAxisd{angle_rad, axis.normalized()}};
}

void expectVectorNear(
    const Eigen::Vector3d& actual,
    const Eigen::Vector3d& expected) {
    EXPECT_NEAR(actual.x(), expected.x(), kTolerance);
    EXPECT_NEAR(actual.y(), expected.y(), kTolerance);
    EXPECT_NEAR(actual.z(), expected.z(), kTolerance);
}

TEST(RotationUtilitiesTest, IdentityRotationMatchesPythonFixedVector) {
    const Eigen::Quaterniond identity{1.0, 0.0, 0.0, 0.0};
    const Eigen::Vector3d vector_body{1.5, -2.0, 4.25};

    expectVectorNear(
        aerial_control::rotateBodyToNed(identity, vector_body),
        vector_body);
    expectVectorNear(
        aerial_control::rotateNedToBody(identity, vector_body),
        vector_body);
}

TEST(RotationUtilitiesTest, QuarterTurnAboutDownAxisRotatesNorthToEast) {
    const double half_sqrt_two = std::sqrt(0.5);
    // Eigen::Quaterniond constructs from (w, x, y, z).
    const Eigen::Quaterniond q_nb{
        half_sqrt_two, 0.0, 0.0, half_sqrt_two};

    expectVectorNear(
        aerial_control::rotateBodyToNed(
            q_nb, Eigen::Vector3d{1.0, 0.0, 0.0}),
        Eigen::Vector3d{0.0, 1.0, 0.0});
}

TEST(RotationUtilitiesTest, CompositionAppliesFirstRotationThenSecond) {
    const Eigen::Quaterniond q_x = axisAngleQuaternion(
        Eigen::Vector3d::UnitX(), std::acos(-1.0) / 2.0);
    const Eigen::Quaterniond q_z = axisAngleQuaternion(
        Eigen::Vector3d::UnitZ(), std::acos(-1.0) / 2.0);
    const Eigen::Vector3d vector_body{0.0, 1.0, 0.0};

    const Eigen::Vector3d sequential = aerial_control::rotateBodyToNed(
        q_z,
        aerial_control::rotateBodyToNed(q_x, vector_body));
    const Eigen::Quaterniond composed =
        aerial_control::composeRotation(q_x, q_z);
    const Eigen::Vector3d composed_result =
        aerial_control::rotateBodyToNed(composed, vector_body);

    expectVectorNear(composed_result, sequential);
    expectVectorNear(composed_result, Eigen::Vector3d{0.0, 0.0, 1.0});
    EXPECT_NEAR(composed.w(), 0.5, kTolerance);
    EXPECT_NEAR(composed.x(), 0.5, kTolerance);
    EXPECT_NEAR(composed.y(), 0.5, kTolerance);
    EXPECT_NEAR(composed.z(), 0.5, kTolerance);
}

TEST(RotationUtilitiesTest, BodyAndNedTransformsAreMutualInverses) {
    const Eigen::Quaterniond q_nb = axisAngleQuaternion(
        Eigen::Vector3d{0.3, 0.7, -0.2}, -0.83);
    const Eigen::Vector3d vector_body{-1.0, 2.5, 0.75};

    const Eigen::Vector3d vector_ned =
        aerial_control::rotateBodyToNed(q_nb, vector_body);
    const Eigen::Vector3d recovered_body =
        aerial_control::rotateNedToBody(q_nb, vector_ned);

    expectVectorNear(recovered_body, vector_body);
    EXPECT_NEAR(vector_ned.norm(), vector_body.norm(), kTolerance);
}

TEST(RotationUtilitiesTest, NormalizationUsesScalarFirstConstructorOrder) {
    const Eigen::Quaterniond input{2.0, -2.0, 1.0, 3.0};
    const Eigen::Quaterniond normalized =
        aerial_control::normalizeQuaternion(input);
    const double expected_norm = std::sqrt(18.0);

    EXPECT_NEAR(normalized.norm(), 1.0, kTolerance);
    EXPECT_NEAR(normalized.w(), 2.0 / expected_norm, kTolerance);
    EXPECT_NEAR(normalized.x(), -2.0 / expected_norm, kTolerance);
    EXPECT_NEAR(normalized.y(), 1.0 / expected_norm, kTolerance);
    EXPECT_NEAR(normalized.z(), 3.0 / expected_norm, kTolerance);
}

TEST(RotationUtilitiesTest, QuaternionAndNegationRotateIdentically) {
    const Eigen::Quaterniond q_nb = axisAngleQuaternion(
        Eigen::Vector3d::UnitY(), 0.61);
    const Eigen::Quaterniond negative_q_nb{
        -q_nb.w(), -q_nb.x(), -q_nb.y(), -q_nb.z()};
    const Eigen::Vector3d vector_body{1.0, 2.0, 3.0};

    expectVectorNear(
        aerial_control::rotateBodyToNed(q_nb, vector_body),
        aerial_control::rotateBodyToNed(negative_q_nb, vector_body));
}

TEST(RotationUtilitiesTest, RejectsInvalidQuaternionAndVectorInputs) {
    const Eigen::Quaterniond zero{0.0, 0.0, 0.0, 0.0};
    const Eigen::Quaterniond non_finite{
        1.0, 0.0, std::numeric_limits<double>::infinity(), 0.0};
    const Eigen::Vector3d invalid_vector{
        1.0, std::numeric_limits<double>::quiet_NaN(), 3.0};

    EXPECT_THROW(
        aerial_control::normalizeQuaternion(zero), std::invalid_argument);
    EXPECT_THROW(
        aerial_control::normalizeQuaternion(non_finite),
        std::invalid_argument);
    EXPECT_THROW(
        aerial_control::rotateBodyToNed(
            Eigen::Quaterniond::Identity(), invalid_vector),
        std::invalid_argument);
}

}  // namespace
