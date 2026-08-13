#include "core/rotation.hpp"

#include <cmath>
#include <limits>
#include <stdexcept>
#include <string>

namespace aerial_control {
namespace {

// 检查四元数是否能够表示有效旋转，并返回它的单位化副本。
// argument_name 只用于构造更明确的异常信息，不参与数学计算。
Eigen::Quaterniond normalizeChecked(
    const Eigen::Quaterniond& quaternion,
    const char* argument_name) {
    // NaN 和正负无穷大都不是合法姿态系数；若继续计算会让错误扩散到后续状态。
    if (!quaternion.coeffs().allFinite()) {
        throw std::invalid_argument(
            std::string{argument_name} + " must contain only finite values");
    }

    // 非零四元数才能归一化。这里同时防止 norm 本身因异常输入成为非有限值。
    const double norm = quaternion.norm();
    if (!std::isfinite(norm) ||
        norm <= std::numeric_limits<double>::epsilon()) {
        throw std::invalid_argument(
            std::string{argument_name} +
            " must have a non-zero norm to represent a rotation");
    }

    // 必须使用命名访问器：Eigen::Quaterniond(w, x, y, z) 的构造顺序，
    // 与 Quaterniond::coeffs() 暴露的 (x, y, z, w) 顺序不同。
    return Eigen::Quaterniond{
        quaternion.w() / norm,
        quaternion.x() / norm,
        quaternion.y() / norm,
        quaternion.z() / norm,
    };
}

// 检查三维向量的每个分量是否均为有限值，防止 NaN 或无穷大进入旋转运算。
void validateVector(
    const Eigen::Vector3d& vector,
    const char* argument_name) {
    if (!vector.allFinite()) {
        throw std::invalid_argument(
            std::string{argument_name} + " must contain only finite values");
    }
}

}  // 结束匿名命名空间

Eigen::Quaterniond normalizeQuaternion(
    const Eigen::Quaterniond& quaternion) {
    // 对外接口复用内部检查函数，并用稳定参数名生成异常信息。
    return normalizeChecked(quaternion, "quaternion");
}

Eigen::Quaterniond composeRotation(
    const Eigen::Quaterniond& first_rotation,
    const Eigen::Quaterniond& second_rotation) {
    // 分别归一化两个输入，避免输入范数漂移在乘法中引入缩放误差。
    const Eigen::Quaterniond first =
        normalizeChecked(first_rotation, "first_rotation");
    const Eigen::Quaterniond second =
        normalizeChecked(second_rotation, "second_rotation");

    // 列向量与 Hamilton 约定下，最右侧旋转最先作用：
    // “先 first、再 second”的组合必须写成 second * first。
    // 相乘后再次归一化，用于消除浮点乘法产生的微小范数漂移。
    return normalizeChecked(second * first, "composed rotation");
}

Eigen::Vector3d rotateBodyToNed(
    const Eigen::Quaterniond& q_nb,
    const Eigen::Vector3d& vector_body) {
    // q_nb 表示从 FRD 机体系 B 到 NED 世界系 N 的旋转。
    validateVector(vector_body, "vector_body");
    const Eigen::Quaterniond unit_q_nb = normalizeChecked(q_nb, "q_nb");
    return unit_q_nb * vector_body;
}

Eigen::Vector3d rotateNedToBody(
    const Eigen::Quaterniond& q_nb,
    const Eigen::Vector3d& vector_ned) {
    validateVector(vector_ned, "vector_ned");
    const Eigen::Quaterniond unit_q_nb = normalizeChecked(q_nb, "q_nb");
    // 单位四元数的共轭等于其逆，因此共轭可执行 NED 到机体系的反向变换。
    return unit_q_nb.conjugate() * vector_ned;
}

}  // 结束 aerial_control 命名空间
