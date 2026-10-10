#include "core/integrators.hpp"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <vector>

namespace aerial_control
{

  struct AngularRateSample
  {
    double timeS;
    StateVector angularRateBodyRadps;
  };
} // namespace aerial_control

int main()
{
  const aerial_control::StateVector initialAngularRateBodyRadps = aerial_control::StateVector::Zero(1);
  aerial_control::StateVector state = initialAngularRateBodyRadps;
  const double initialAngularRateXRadps = initialAngularRateBodyRadps(0);
  const double inertiaXKgM2 = 0.02;
  const double momentXNm = 0.01;
  const double dtS = 0.01;
  const std::size_t numIntegrationSteps = 200U;

  const aerial_control::DerivativeFunction derivative = [inertiaXKgM2, momentXNm](double, const aerial_control::StateVector &) -> aerial_control::StateVector
  {
    aerial_control::StateVector angularAccelerationBodyRadps2 = aerial_control::StateVector::Zero(1);
    angularAccelerationBodyRadps2(0) = momentXNm / inertiaXKgM2;
    return angularAccelerationBodyRadps2;
  };

  std::vector<aerial_control::AngularRateSample> samples;
  samples.reserve(numIntegrationSteps + 1U); 
  samples.push_back({0.0, state});

  double finalTimeS = 0.0;
  std::size_t numCompletedIntegrationSteps = 0U;
  for (std::size_t i = 0; i < numIntegrationSteps; ++i)
  {
    const double timeS = static_cast<double>(i) * dtS;
    state = aerial_control::rk4Step(derivative, timeS, state, dtS);
    ++numCompletedIntegrationSteps;

    const double nextTimeS = static_cast<double>(i + 1U) * dtS;
    samples.push_back({nextTimeS, state});
    finalTimeS = nextTimeS;
  }

  double maxAbsoluteErrorRadps = 0.0;
  bool allSamplesFinite = true;
  for (const aerial_control::AngularRateSample &sample : samples)
  {
    allSamplesFinite = allSamplesFinite && std::isfinite(sample.timeS) && sample.angularRateBodyRadps.allFinite();
    const double analyticAngularRateXRadps = initialAngularRateXRadps + (momentXNm / inertiaXKgM2) * sample.timeS;
    maxAbsoluteErrorRadps = std::max(maxAbsoluteErrorRadps, std::abs(sample.angularRateBodyRadps(0) - analyticAngularRateXRadps));
  }

  const aerial_control::AngularRateSample &initialSample = samples.front();
  const aerial_control::AngularRateSample &finalSample = samples.back();
  const aerial_control::StateVector &finalAngularRateBodyRadps = finalSample.angularRateBodyRadps;
  const bool passed = numCompletedIntegrationSteps == numIntegrationSteps && samples.size() == numIntegrationSteps + 1U && allSamplesFinite && initialSample.timeS == 0.0 && initialSample.angularRateBodyRadps(0) == initialAngularRateXRadps && std::abs(finalTimeS - 2.0) <= 1e-12 && std::abs(finalSample.timeS - finalTimeS) <= 1e-12 && std::abs(finalAngularRateBodyRadps(0) - state(0)) <= 1e-12 && maxAbsoluteErrorRadps <= 1e-10;

  std::cout << std::setprecision(17);
  std::cout << "integrations=" << numCompletedIntegrationSteps << '\n' << "samples=" << samples.size() << '\n';
  std::cout << "initial_sample_time_s=" << initialSample.timeS << '\n' << "initial_sample_angular_rate_x_radps=" << initialSample.angularRateBodyRadps(0) << '\n';
  std::cout << "final_time_s=" << finalTimeS << '\n' << "final_sample_time_s=" << finalSample.timeS << '\n';
  std::cout << "final_state_angular_rate_x_radps=" << state(0) << '\n' << "final_sample_angular_rate_x_radps=" << finalAngularRateBodyRadps(0) << '\n';
  std::cout << "all_samples_finite=" << std::boolalpha << allSamplesFinite << '\n';
  std::cout << "max_absolute_error_radps=" << maxAbsoluteErrorRadps << '\n';
  std::cout << "validation=" << (passed ? "PASS" : "FAIL") << '\n';

  return passed ? 0 : 1;
}
