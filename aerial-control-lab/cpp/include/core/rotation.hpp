#pragma once

#include <Eigen/Geometry>

namespace aerial_control {

/// 返回表示同一旋转的单位四元数。
///
/// Eigen 的构造函数按 (w, x, y, z) 接收参数，而 coeffs() 按
/// (x, y, z, w) 暴露系数。调用者不得假定两种表示的下标顺序相同，
/// 也不得直接按相同下标在二者之间复制系数。
///
/// 当任一系数不是有限值，或四元数范数过于接近零、无法定义旋转时，
/// 抛出 std::invalid_argument。
Eigen::Quaterniond normalizeQuaternion(
    const Eigen::Quaterniond& quaternion);

/// 合成两个旋转并返回单位四元数。
///
/// 返回结果表示先执行 first_rotation，再执行 second_rotation。
/// 在本项目的列向量与 Hamilton 乘法约定下，合成顺序为
/// 具体写作 second_rotation * first_rotation。
///
/// 无效输入的判断和异常策略与 normalizeQuaternion() 相同。
Eigen::Quaterniond composeRotation(
    const Eigen::Quaterniond& first_rotation,
    const Eigen::Quaterniond& second_rotation);

/// 将向量从 FRD 机体系 B 转换到 NED 世界系 N。
///
/// 函数会在内部归一化 q_nb。若 q_nb 无效，或 vector_body 含有非有限分量，
/// 则抛出 std::invalid_argument。
Eigen::Vector3d rotateBodyToNed(
    const Eigen::Quaterniond& q_nb,
    const Eigen::Vector3d& vector_body);

/// 将向量从 NED 世界系 N 转换到 FRD 机体系 B。
///
/// 该函数应用 q_nb 的逆旋转，并在内部归一化 q_nb。若 q_nb 无效，
/// 或 vector_ned 含有非有限分量，则抛出 std::invalid_argument。
Eigen::Vector3d rotateNedToBody(
    const Eigen::Quaterniond& q_nb,
    const Eigen::Vector3d& vector_ned);

}  // 结束 aerial_control 命名空间
