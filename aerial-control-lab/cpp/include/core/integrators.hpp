#pragma once

#include <Eigen/Core>

#include <functional>

namespace aerial_control {

using StateVector = Eigen::VectorXd;
using DerivativeFunction =
    std::function<StateVector(double, const StateVector&)>;

/// 使用显式 Euler 方法推进一个时间步。
///
/// state 必须非空且所有分量有限，dt 必须为正有限值。derivative 在当前
/// 时间与状态处求值，并且必须返回同维、有限的状态导数；否则抛出
/// std::invalid_argument。函数不修改调用者传入的 state。
StateVector eulerStep(
    const DerivativeFunction& derivative,
    double t,
    const StateVector& state,
    double dt);

/// 使用经典四阶 Runge-Kutta 方法推进一个时间步。
///
/// 输入、每个中间状态以及四次导数求值均执行与 eulerStep 相同的有限性
/// 和维度检查。函数不包含任何具体动力学模型逻辑，也不修改输入状态。
StateVector rk4Step(
    const DerivativeFunction& derivative,
    double t,
    const StateVector& state,
    double dt);

}  // namespace aerial_control
