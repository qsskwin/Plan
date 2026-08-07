#pragma once

#include <Eigen/Geometry>

namespace aerial_control {

/// Return a unit quaternion representing the same rotation.
///
/// Eigen's constructor uses (w, x, y, z), while coeffs() exposes
/// (x, y, z, w). Callers must not copy coefficients by index across those
/// two representations.
///
/// Throws std::invalid_argument when any coefficient is non-finite or the
/// quaternion norm is too close to zero to define a rotation.
Eigen::Quaterniond normalizeQuaternion(
    const Eigen::Quaterniond& quaternion);

/// Compose two rotations and return a unit quaternion.
///
/// The returned rotation applies first_rotation first and second_rotation
/// second. With the project's column-vector/Hamilton convention this is
/// second_rotation * first_rotation.
///
/// Throws std::invalid_argument under the same conditions as
/// normalizeQuaternion().
Eigen::Quaterniond composeRotation(
    const Eigen::Quaterniond& first_rotation,
    const Eigen::Quaterniond& second_rotation);

/// Transform a vector from the FRD body frame B to the NED frame N.
///
/// q_nb is normalized internally. Throws std::invalid_argument if q_nb is
/// invalid or vector_body contains a non-finite component.
Eigen::Vector3d rotateBodyToNed(
    const Eigen::Quaterniond& q_nb,
    const Eigen::Vector3d& vector_body);

/// Transform a vector from the NED frame N to the FRD body frame B.
///
/// This applies the inverse of q_nb. q_nb is normalized internally. Throws
/// std::invalid_argument if q_nb is invalid or vector_ned contains a
/// non-finite component.
Eigen::Vector3d rotateNedToBody(
    const Eigen::Quaterniond& q_nb,
    const Eigen::Vector3d& vector_ned);

}  // namespace aerial_control
