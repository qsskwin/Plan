#include "coordinate_conversions.hpp"

namespace coordinate_conversions {
    Eigen::Vector3d nedToEnu(const Eigen::Vector3d& ned) {
        return Eigen::Vector3d(ned.y(), ned.x(), -ned.z());
    }

    Eigen::Vector3d enuToNed(const Eigen::Vector3d& enu) {
        return Eigen::Vector3d(enu.y(), enu.x(), -enu.z());
    }

    Eigen::Vector3d frdToFlu(const Eigen::Vector3d& frd) {
        return Eigen::Vector3d(frd.x(), -frd.y(), -frd.z());
    }

    Eigen::Vector3d fluToFrd(const Eigen::Vector3d& flu) {
        return Eigen::Vector3d(flu.x(), -flu.y(), -flu.z());
    }
} // namespace coordinate_conversions