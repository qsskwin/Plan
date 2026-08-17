#pragma once

#include <Eigen/Core>

namespace aerial_control::week2_mon {

/// 将 `value` 限制在闭区间 `[lower_bound, upper_bound]` 内。
///
/// 当 `lower_bound` 大于 `upper_bound` 时，抛出 `std::invalid_argument`。
double ClampValue(double value, double lower_bound, double upper_bound);

/// 检查三维向量的所有分量是否均为有限值。
///
/// 所有分量均不是 NaN 或正负无穷时返回 `true`，否则返回 `false`。
bool IsFiniteVector(const Eigen::Vector3d& vector);

}  // namespace aerial_control::week2_mon
