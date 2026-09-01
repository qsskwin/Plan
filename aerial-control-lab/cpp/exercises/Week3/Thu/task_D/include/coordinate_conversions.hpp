#pragma once

#include <Eigen/Core>

namespace coordinate_conversions {
    Eigen::Vector3d nedToEnu(const Eigen::Vector3d& ned);
    Eigen::Vector3d enuToNed(const Eigen::Vector3d& enu);
    Eigen::Vector3d frdToFlu(const Eigen::Vector3d& frd);
    Eigen::Vector3d fluToFrd(const Eigen::Vector3d& flu);
} // namespace coordinate_conversions
