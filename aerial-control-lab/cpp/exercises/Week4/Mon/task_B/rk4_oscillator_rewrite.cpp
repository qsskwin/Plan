
#include "core/integrators.hpp"

#include <cmath>
#include <functional>
#include <iomanip>
#include <iostream>

// 调用链：
// main
//   -> simulate(rk4StepIndependent 或 aerial_control::rk4Step)
//      -> 单步函数
//         -> oscillatorDerivative（RK4 每一步调用四次）

using StepFunction = std::function<aerial_control::StateVector(
    const aerial_control::DerivativeFunction&,
    double,
    const aerial_control::StateVector&,
    double)>;

constexpr double kInitialTime = 0.0;
constexpr double kTimeStep = 0.1;
constexpr int kStepCount = 10;
constexpr double kEndTime = kInitialTime + kTimeStep * kStepCount;

// 第一层：具体动力学。输入状态 [q, v]，返回状态导数 [v, -q]。
aerial_control::StateVector oscillatorDerivative(
    double /*t*/, const aerial_control::StateVector& state) {
  aerial_control::StateVector result(2);
  result << state(1), -state(0);
  return result;
}

// 第二层：通用 RK4 单步。它通过 derivative 获取动力学，不了解简谐振子细节。
aerial_control::StateVector rk4StepIndependent(
    const aerial_control::DerivativeFunction& derivative,
    double t,
    const aerial_control::StateVector& state,
    double dt) {
  const aerial_control::StateVector k1 = derivative(t, state);
  const aerial_control::StateVector k2 =
      derivative(t + dt / 2, state + dt / 2 * k1);
  const aerial_control::StateVector k3 =
      derivative(t + dt / 2, state + dt / 2 * k2);
  const aerial_control::StateVector k4 =
      derivative(t + dt, state + dt * k3);

  return state + dt / 6 * (k1 + 2 * k2 + 2 * k3 + k4);
}

// 第三层：仿真循环。它只负责重复调用传入的单步函数并推进时间。
aerial_control::StateVector simulate(const StepFunction& stepFunction) {
  aerial_control::StateVector state(2);
  state << 1.0, 0.0;

  double t = kInitialTime;

  for (int i = 0; i < kStepCount; ++i) {
    state = stepFunction(oscillatorDerivative, t, state, kTimeStep);
    t += kTimeStep;
  }

  return state;
}
 
// 第四层：程序入口。选择两种单步函数，运行相同仿真并比较结果。
int main() {
  const aerial_control::StateVector independentResult =
      simulate(rk4StepIndependent);
  const aerial_control::StateVector officialResult =
      simulate(aerial_control::rk4Step);

  aerial_control::StateVector analyticalResult(2);
  analyticalResult << std::cos(kEndTime), -std::sin(kEndTime);

  const double independentAnalyticalError =
      (independentResult - analyticalResult).cwiseAbs().maxCoeff();
  const double independentOfficialDifference =
      (independentResult - officialResult).cwiseAbs().maxCoeff();

  std::cout << std::setprecision(17);
  std::cout << "Independent RK4 result: " << independentResult.transpose()
            << '\n';
  std::cout << "Official RK4 result: " << officialResult.transpose() << '\n';
  std::cout << "Analytical result: " << analyticalResult.transpose() << '\n';
  std::cout << "Independent vs analytical max error: "
            << independentAnalyticalError << '\n';
  std::cout << "Independent vs official max difference: "
            << independentOfficialDifference << '\n';

  return 0;
}
