#include <Eigen/Core>
#include <Eigen/Geometry>
#include <gtest/gtest.h>

#include <cmath>
#include <limits>
#include <stdexcept>

#include "core/rotation.hpp"

namespace {

// Python 与 C++ 共享测试向量规定的浮点绝对误差容限。
constexpr double kTolerance = 1e-12;

// 根据单位旋转轴和旋转角构造 Eigen 四元数，供各测试复用。
Eigen::Quaterniond axisAngleQuaternion(
    const Eigen::Vector3d& axis,
    const double angle_rad) {
    return Eigen::Quaterniond{Eigen::AngleAxisd{angle_rad, axis.normalized()}};
}

// 逐分量比较两个三维向量，避免对浮点计算结果直接使用精确相等判断。
void expectVectorNear(
    const Eigen::Vector3d& actual,
    const Eigen::Vector3d& expected) {
    EXPECT_NEAR(actual.x(), expected.x(), kTolerance);
    EXPECT_NEAR(actual.y(), expected.y(), kTolerance);
    EXPECT_NEAR(actual.z(), expected.z(), kTolerance);
}

TEST(RotationUtilitiesTest, IdentityRotationMatchesPythonFixedVector) {
    // 单位四元数不应改变向量，正向与反向变换都必须返回原值。
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
    // Eigen::Quaterniond 的构造参数顺序为 (w, x, y, z)。
    const Eigen::Quaterniond q_nb{
        half_sqrt_two, 0.0, 0.0, half_sqrt_two};

    // 绕 NED 向下轴正转 90° 后，机体前向（北）应转到东向。
    expectVectorNear(
        aerial_control::rotateBodyToNed(
            q_nb, Eigen::Vector3d{1.0, 0.0, 0.0}),
        Eigen::Vector3d{0.0, 1.0, 0.0});
}

TEST(RotationUtilitiesTest, CompositionAppliesFirstRotationThenSecond) {
    // 固定顺序：先绕 x 轴正转 90°，再绕 z 轴正转 90°。
    const Eigen::Quaterniond q_x = axisAngleQuaternion(
        Eigen::Vector3d::UnitX(), std::acos(-1.0) / 2.0);
    const Eigen::Quaterniond q_z = axisAngleQuaternion(
        Eigen::Vector3d::UnitZ(), std::acos(-1.0) / 2.0);
    const Eigen::Vector3d vector_body{0.0, 1.0, 0.0};

    // sequential 显式执行两次旋转，作为组合结果的独立对照。
    const Eigen::Vector3d sequential = aerial_control::rotateBodyToNed(
        q_z,
        aerial_control::rotateBodyToNed(q_x, vector_body));
    const Eigen::Quaterniond composed =
        aerial_control::composeRotation(q_x, q_z);
    const Eigen::Vector3d composed_result =
        aerial_control::rotateBodyToNed(composed, vector_body);

    // 合成后旋转一次必须与顺序执行两次旋转一致，并符合共享固定向量的期望值。
    expectVectorNear(composed_result, sequential);
    expectVectorNear(composed_result, Eigen::Vector3d{0.0, 0.0, 1.0});
    EXPECT_NEAR(composed.w(), 0.5, kTolerance);
    EXPECT_NEAR(composed.x(), 0.5, kTolerance);
    EXPECT_NEAR(composed.y(), 0.5, kTolerance);
    EXPECT_NEAR(composed.z(), 0.5, kTolerance);
}

TEST(RotationUtilitiesTest, BodyAndNedTransformsAreMutualInverses) {
    // 使用非坐标轴方向和非特殊角度，检查正向/逆向变换与向量范数保持性质。
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
    // 该固定输入同时检查归一化结果和 Eigen 标量在前的构造顺序。
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
    // q 与 -q 是单位四元数对同一三维姿态的双覆盖，旋转结果必须相同。
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
    // 零范数、无穷大和 NaN 都必须明确抛出异常，不能静默生成错误姿态。
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

}  // 结束匿名命名空间
