#include <Eigen/Core>
#include <gtest/gtest.h>

TEST(EigenVectorTest, AddsThreeDimensionalVectors) {
    const Eigen::Vector3d first{1.0, 2.0, 3.0};
    const Eigen::Vector3d second{-1.0, 4.0, 0.5};

    const Eigen::Vector3d result = first + second;

    EXPECT_DOUBLE_EQ(result.x(), 0.0);
    EXPECT_DOUBLE_EQ(result.y(), 6.0);
    // 接入阶段曾故意将期望值写成 4.0；GoogleTest 正确报告实际值 3.5、
    // 错误期望值和源码位置。最终测试保留修正后的真实结果。
    EXPECT_DOUBLE_EQ(result.z(), 3.5);
}

TEST(EigenVectorTest, ComputesDotProductAndNorm) {
    const Eigen::Vector3d vector{3.0, 4.0, 0.0};
    const Eigen::Vector3d unit_x = Eigen::Vector3d::UnitX();

    EXPECT_DOUBLE_EQ(vector.dot(unit_x), 3.0);
    EXPECT_NEAR(vector.norm(), 5.0, 1e-12);
}
