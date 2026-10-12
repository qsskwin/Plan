#include "core/integrators.hpp"
#include "core/rotational_dynamics.hpp"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <exception>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace aerial_control
{

  struct AngularRateCase
  {
    std::string name;
    StateVector initialAngularRateBodyRadps;
    RotationalParameters parameters;
    RotationalInput input;
    double dtS;
    std::size_t numIntegrationSteps;
  };

  struct AngularRateSample
  {
    double timeS;
    StateVector angularRateBodyRadps;
  };

  std::vector<AngularRateSample> runAngularRateCase(const AngularRateCase &angularRateCase)
  {
    if (!std::isfinite(angularRateCase.dtS) || angularRateCase.dtS <= 0.0)
    {
      throw std::invalid_argument("dtS must be positive and finite");
    }

    const double finalTimeS = static_cast<double>(angularRateCase.numIntegrationSteps) * angularRateCase.dtS;
    if (!std::isfinite(finalTimeS))
    {
      throw std::invalid_argument("final time must be finite");
    }

    StateVector state = angularRateCase.initialAngularRateBodyRadps;
    const RotationalParameters parameters = angularRateCase.parameters;
    const RotationalInput input = angularRateCase.input;

    // 固定参数与力矩按值捕获，每次使用 RK4 提供的求值状态。
    const DerivativeFunction derivative = [parameters, input](double timeS, const StateVector &evaluationState) -> StateVector
    {
      return angularRateDerivative(timeS, evaluationState, parameters, input);
    };

    // 复用正式导数的输入检查，零步工况也检查初态、惯量和力矩。
    static_cast<void>(derivative(0.0, state));

    std::vector<AngularRateSample> samples;
    if (angularRateCase.numIntegrationSteps >= samples.max_size())
    {
      throw std::invalid_argument("integration steps exceed sample capacity");
    }
    samples.reserve(angularRateCase.numIntegrationSteps + 1U);
    samples.push_back({0.0, state});

    for (std::size_t i = 0; i < angularRateCase.numIntegrationSteps; ++i)
    {
      const double timeS = static_cast<double>(i) * angularRateCase.dtS;
      state = rk4Step(derivative, timeS, state, angularRateCase.dtS);

      // 更新后的角速度按值保存，并标注本步终点时间。
      const double nextTimeS = static_cast<double>(i + 1U) * angularRateCase.dtS;
      samples.push_back({nextTimeS, state});
    }

    return samples;
  }
} // namespace aerial_control

int main()
{
  try
  {
    const aerial_control::RotationalParameters parameters{Eigen::Vector3d{0.02, 0.03, 0.04}};
    const aerial_control::RotationalInput zeroMoment{Eigen::Vector3d::Zero()};
    const aerial_control::AngularRateCase stationary{"stationary", aerial_control::StateVector::Zero(3), parameters, zeroMoment, 0.01, 200U};
    const aerial_control::StateVector initialPrincipalAxisRateBodyRadps = Eigen::Vector3d{0.0, 0.0, 1.0};
    const aerial_control::AngularRateCase principalAxisRotation{"torque-free principal-axis rotation", initialPrincipalAxisRateBodyRadps, parameters, zeroMoment, 0.01, 200U};
    const aerial_control::RotationalInput positiveRollMoment{Eigen::Vector3d{0.01, 0.0, 0.0}};
    const aerial_control::AngularRateCase positiveRoll{"constant positive roll moment", aerial_control::StateVector::Zero(3), parameters, positiveRollMoment, 0.01, 200U};

    std::cout << std::setprecision(17) << std::boolalpha;
    bool allCasesPassed = true;
    for (const aerial_control::AngularRateCase &angularRateCase : {stationary, principalAxisRotation, positiveRoll})
    {
      const std::vector<aerial_control::AngularRateSample> samples = aerial_control::runAngularRateCase(angularRateCase);
      const aerial_control::StateVector initialAngularAccelerationBodyRadps2 = aerial_control::angularRateDerivative(0.0, angularRateCase.initialAngularRateBodyRadps, angularRateCase.parameters, angularRateCase.input);
      // 前三个正式工况均无耦合，使用各自的常量角加速度解析解。
      const aerial_control::StateVector analyticAngularAccelerationBodyRadps2 = angularRateCase.input.momentBodyNm.cwiseQuotient(angularRateCase.parameters.principalInertiaKgM2);

      double maxAbsoluteErrorRadps = 0.0;
      bool allSamplesFinite = true;
      bool allSampleTimesAligned = true;
      for (std::size_t i = 0; i < samples.size(); ++i)
      {
        const aerial_control::AngularRateSample &sample = samples[i];
        allSamplesFinite = allSamplesFinite && std::isfinite(sample.timeS) && sample.angularRateBodyRadps.size() == 3 && sample.angularRateBodyRadps.allFinite();
        allSampleTimesAligned = allSampleTimesAligned && sample.timeS == static_cast<double>(i) * angularRateCase.dtS;
        const aerial_control::StateVector analyticAngularRateBodyRadps = angularRateCase.initialAngularRateBodyRadps + sample.timeS * analyticAngularAccelerationBodyRadps2;
        maxAbsoluteErrorRadps = std::max(maxAbsoluteErrorRadps, (sample.angularRateBodyRadps - analyticAngularRateBodyRadps).cwiseAbs().maxCoeff());
      }

      const aerial_control::AngularRateSample &initialSample = samples.front();
      const aerial_control::AngularRateSample &finalSample = samples.back();
      const bool initialStateMatches = (initialSample.angularRateBodyRadps.array() == angularRateCase.initialAngularRateBodyRadps.array()).all();
      const bool initialDerivativeMatches = (initialAngularAccelerationBodyRadps2 - analyticAngularAccelerationBodyRadps2).cwiseAbs().maxCoeff() <= 1e-12;
      const bool passed = samples.size() == angularRateCase.numIntegrationSteps + 1U && allSamplesFinite && allSampleTimesAligned && initialSample.timeS == 0.0 && initialStateMatches && initialDerivativeMatches && std::abs(finalSample.timeS - 2.0) <= 1e-12 && maxAbsoluteErrorRadps <= 1e-10;
      allCasesPassed = allCasesPassed && passed;

      std::cout << "case=" << angularRateCase.name << '\n';
      std::cout << "integrations=" << samples.size() - 1U << '\n' << "samples=" << samples.size() << '\n';
      std::cout << "initial_sample_time_s=" << initialSample.timeS << '\n' << "initial_angular_rate_body_radps=" << initialSample.angularRateBodyRadps.transpose() << '\n';
      std::cout << "initial_angular_acceleration_body_radps2=" << initialAngularAccelerationBodyRadps2.transpose() << '\n';
      std::cout << "final_time_s=" << finalSample.timeS << '\n' << "final_angular_rate_body_radps=" << finalSample.angularRateBodyRadps.transpose() << '\n';
      std::cout << "all_samples_finite=" << allSamplesFinite << '\n' << "all_sample_times_aligned=" << allSampleTimesAligned << '\n';
      std::cout << "max_absolute_error_radps=" << maxAbsoluteErrorRadps << '\n';
      std::cout << "validation=" << (passed ? "PASS" : "FAIL") << "\n\n";
    }

    return allCasesPassed ? 0 : 1;
  }
  catch (const std::exception &error)
  {
    std::cerr << "angular_rate_demo: " << error.what() << '\n';
    return 1;
  }
}
