#include "core/rotation.hpp"

#include <cmath>
#include <limits>
#include <stdexcept>
#include <string>

namespace aerial_control {
namespace {

Eigen::Quaterniond normalizeChecked(
    const Eigen::Quaterniond& quaternion,
    const char* argument_name) {
    if (!quaternion.coeffs().allFinite()) {
        throw std::invalid_argument(
            std::string{argument_name} + " must contain only finite values");
    }

    const double norm = quaternion.norm();
    if (!std::isfinite(norm) ||
        norm <= std::numeric_limits<double>::epsilon()) {
        throw std::invalid_argument(
            std::string{argument_name} +
            " must have a non-zero norm to represent a rotation");
    }

    // Use named accessors because Eigen::Quaterniond(w, x, y, z) and
    // Quaterniond::coeffs() expose different coefficient orders.
    return Eigen::Quaterniond{
        quaternion.w() / norm,
        quaternion.x() / norm,
        quaternion.y() / norm,
        quaternion.z() / norm,
    };
}

void validateVector(
    const Eigen::Vector3d& vector,
    const char* argument_name) {
    if (!vector.allFinite()) {
        throw std::invalid_argument(
            std::string{argument_name} + " must contain only finite values");
    }
}

}  // namespace

Eigen::Quaterniond normalizeQuaternion(
    const Eigen::Quaterniond& quaternion) {
    return normalizeChecked(quaternion, "quaternion");
}

Eigen::Quaterniond composeRotation(
    const Eigen::Quaterniond& first_rotation,
    const Eigen::Quaterniond& second_rotation) {
    const Eigen::Quaterniond first =
        normalizeChecked(first_rotation, "first_rotation");
    const Eigen::Quaterniond second =
        normalizeChecked(second_rotation, "second_rotation");

    return normalizeChecked(second * first, "composed rotation");
}

Eigen::Vector3d rotateBodyToNed(
    const Eigen::Quaterniond& q_nb,
    const Eigen::Vector3d& vector_body) {
    validateVector(vector_body, "vector_body");
    const Eigen::Quaterniond unit_q_nb = normalizeChecked(q_nb, "q_nb");
    return unit_q_nb * vector_body;
}

Eigen::Vector3d rotateNedToBody(
    const Eigen::Quaterniond& q_nb,
    const Eigen::Vector3d& vector_ned) {
    validateVector(vector_ned, "vector_ned");
    const Eigen::Quaterniond unit_q_nb = normalizeChecked(q_nb, "q_nb");
    return unit_q_nb.conjugate() * vector_ned;
}

}  // namespace aerial_control
