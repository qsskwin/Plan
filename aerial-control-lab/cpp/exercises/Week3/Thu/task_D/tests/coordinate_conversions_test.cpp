#include <Eigen/Core>
#include <gtest/gtest.h>

#include "coordinate_conversions.hpp"

namespace {

TEST(CoordinateConversionsTest, ConvertsNedBasisVectorsToEnu) {
    Eigen::Vector3d ned_x(1.0, 0.0, 0.0);
    Eigen::Vector3d ned_y(0.0, 1.0, 0.0);
    Eigen::Vector3d ned_z(0.0, 0.0, 1.0);

    Eigen::Vector3d enu_x = coordinate_conversions::nedToEnu(ned_x);
    Eigen::Vector3d enu_y = coordinate_conversions::nedToEnu(ned_y);
    Eigen::Vector3d enu_z = coordinate_conversions::nedToEnu(ned_z);

    EXPECT_DOUBLE_EQ(enu_x.x(), 0.0);
    EXPECT_DOUBLE_EQ(enu_x.y(), 1.0);
    EXPECT_DOUBLE_EQ(enu_x.z(), 0.0);

    EXPECT_DOUBLE_EQ(enu_y.x(), 1.0);
    EXPECT_DOUBLE_EQ(enu_y.y(), 0.0);
    EXPECT_DOUBLE_EQ(enu_y.z(), 0.0);

    EXPECT_DOUBLE_EQ(enu_z.x(), 0.0);
    EXPECT_DOUBLE_EQ(enu_z.y(), 0.0);
    EXPECT_DOUBLE_EQ(enu_z.z(), -1.0);
}

TEST(CoordinateConversionsTest, ConvertsFrdToFlu) {
    Eigen::Vector3d frd_x(1.0, 0.0, 0.0);
    Eigen::Vector3d frd_y(0.0, 2.0, 0.0);
    Eigen::Vector3d frd_z(0.0, 0.0, 3.0);

    Eigen::Vector3d flu_x = coordinate_conversions::frdToFlu(frd_x);
    Eigen::Vector3d flu_y = coordinate_conversions::frdToFlu(frd_y);
    Eigen::Vector3d flu_z = coordinate_conversions::frdToFlu(frd_z);

    EXPECT_DOUBLE_EQ(flu_x.x(), 1.0);
    EXPECT_DOUBLE_EQ(flu_x.y(), 0.0);
    EXPECT_DOUBLE_EQ(flu_x.z(), 0.0);

    EXPECT_DOUBLE_EQ(flu_y.x(), 0.0);
    EXPECT_DOUBLE_EQ(flu_y.y(), -2.0);
    EXPECT_DOUBLE_EQ(flu_y.z(), 0.0);

    EXPECT_DOUBLE_EQ(flu_z.x(), 0.0);
    EXPECT_DOUBLE_EQ(flu_z.y(), 0.0);
    EXPECT_DOUBLE_EQ(flu_z.z(), -3.0);

    EXPECT_DOUBLE_EQ(coordinate_conversions::fluToFrd(flu_x).x(), 1.0);
    EXPECT_DOUBLE_EQ(coordinate_conversions::fluToFrd(flu_x).y(), 0.0);
    EXPECT_DOUBLE_EQ(coordinate_conversions::fluToFrd(flu_x).z(), 0.0);

    EXPECT_DOUBLE_EQ(coordinate_conversions::fluToFrd(flu_y).x(), 0.0);
    EXPECT_DOUBLE_EQ(coordinate_conversions::fluToFrd(flu_y).y(), 2.0);
    EXPECT_DOUBLE_EQ(coordinate_conversions::fluToFrd(flu_y).z(), 0.0);

    EXPECT_DOUBLE_EQ(coordinate_conversions::fluToFrd(flu_z).x(), 0.0);
    EXPECT_DOUBLE_EQ(coordinate_conversions::fluToFrd(flu_z).y(), 0.0);
    EXPECT_DOUBLE_EQ(coordinate_conversions::fluToFrd(flu_z).z(), 3.0);

    EXPECT_DOUBLE_EQ(frd_x.norm(), flu_x.norm());
    EXPECT_DOUBLE_EQ(frd_y.norm(), flu_y.norm());
    EXPECT_DOUBLE_EQ(frd_z.norm(), flu_z.norm());
}

}  // namespace
