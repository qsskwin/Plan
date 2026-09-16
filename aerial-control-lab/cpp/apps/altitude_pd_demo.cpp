#include "core/altitude_pd.hpp"
#include "core/integrators.hpp"
#include "core/point_mass.hpp"

#include <Eigen/Geometry>

#include <cstddef>
#include <iomanip>
#include <iostream>
#include <iomanip>

int main() {
    // 仿真设置
    constexpr double kDtS = 0.01;
    constexpr std::size_t kNumIntegrationSteps = 1000U;
    constexpr std::size_t kControlStride = 2U;
    constexpr double kInitialReferenceHeightM = 0.0;
    constexpr double kStepReferenceHeightM = 1.0;
    constexpr double kStepTimeS = 1.0;        
    double kReferenceHeightM = 0.0;
    // 实际被控对象参数
    const aerial_control::PointMassParameters plantParameters{
        1.5  // massKg
    };

    // 控制器参数
    const aerial_control::AltitudePdParameters controllerParameters{
        1.5,       // nominalMassKg
        4.0,       // kpPerSecondSquared
        4.0,       // kdPerSecond
        0.0,       // minThrustN
        29.41995  // maxThrustN
    };

    const aerial_control::AltitudePdController controller{
        controllerParameters
    };

    // 六维状态：[p_n, p_e, p_d, v_n, v_e, v_d]
    aerial_control::StateVector state =
        aerial_control::StateVector::Zero(6);
        // state(2) = -1.0;  // 初始高度 1 m
        // state(5) = 1.0;   // 初始垂直速度 0 m/s
    // 在两个控制采样时刻之间保持不变的输入
    aerial_control::PointMassInput heldInput{
        0.0,
        Eigen::Quaterniond{1.0, 0.0, 0.0, 0.0},
    };

    std::size_t integrationCount = 0U;
    std::size_t controlUpdateCount = 0U;

    std::cout << std::setprecision(15);

    // lambda 只读取当前保持的输入，不更新控制器
    const aerial_control::DerivativeFunction dynamics =
        [&plantParameters, &heldInput](
            const double timeS,
            const aerial_control::StateVector& evaluationState) {
            return aerial_control::pointMassDerivative(
                timeS,
                evaluationState,
                plantParameters,
                heldInput);
        };

    for (std::size_t step = 0U;
         step < kNumIntegrationSteps;
         ++step) {
        const double timeS =  static_cast<double>(step) * kDtS;
        
        // TODO 1：
        // 用整数 step 和 kControlStride 判断当前是否为控制采样时刻。
        //
        // 如果需要更新控制器：
        //   a. 从当前 state 计算 heightM = -state(2)
        //   b. 从当前 state 计算 verticalSpeedMps = -state(5)
        //   c. 调用 controller.compute(...)
        //   d. 把 appliedThrustN 写入 heldInput.totalThrustN
        //   e. controlUpdateCount 加一
        if (step % kControlStride == 0U) {

            if (timeS < kStepTimeS) {
                kReferenceHeightM = kInitialReferenceHeightM;
            } else {
                kReferenceHeightM = kStepReferenceHeightM;
            }

                const double heightM = -state(2);
                const double verticalSpeedMps = -state(5);
                
                const aerial_control::AltitudeCommand command =
                    controller.compute(
                        kReferenceHeightM,
                        heightM,
                        verticalSpeedMps);

                heldInput.totalThrustN = command.appliedThrustN;
                ++controlUpdateCount;

                if (step == 98U || step == 100U || step == 102U ||
                    step == 200U || step == 998U) {
                    std::cout << std::setprecision(15);        
                    std::cout << "step=" << step
                              << ", time=" << timeS << " s"
                              << ", reference=" << kReferenceHeightM << " m"
                              << ", height=" << heightM << " m"
                              << ", vertical_speed=" << verticalSpeedMps
                              << " m/s"
                              << ", raw_thrust=" << command.rawThrustN << " N"
                              << ", applied_thrust="
                              << command.appliedThrustN << " N\n";
                }
            }

        // TODO 2：
        // 使用 dynamics、timeS、state 和 kDtS 调用 rk4Step，
        // 并用返回值更新 state。
        state = aerial_control::rk4Step(dynamics, timeS, state, kDtS);
        // TODO 3：
        // integrationCount 加一。
        ++integrationCount;
    }

    std::cout << "integration count: "
              << integrationCount << '\n';
    std::cout << "control update count: "
              << controlUpdateCount << '\n';
    std::cout << "final time: "
              << static_cast<double>(kNumIntegrationSteps) * kDtS
              << " s\n";
    std::cout << "final height: "
              << -state(2) << " m\n";
    std::cout << "final vertical speed: "
              << -state(5) << " m/s\n";
    std::cout << "final north position: "
              << state(0) << " m\n";
    std::cout << "final east position: "
              << state(1) << " m\n";
    std::cout << "final north velocity: "
              << state(3) << " m/s\n";
    std::cout << "final east velocity: "
              << state(4) << " m/s\n";

    return 0;
}
