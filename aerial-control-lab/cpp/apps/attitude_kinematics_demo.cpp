#include "core/attitude_kinematics.hpp"
#include "core/integrators.hpp"
#include "core/rotation.hpp"

#include <Eigen/Geometry>

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

  struct AttitudeCase
  {
    std::string name;
    StateVector initialQuaternionWxyz;
    Eigen::Vector3d angularRateBodyRadps;
    double dtS;
    std::size_t numIntegrationSteps;
  };

  struct AttitudeSample
  {
    double timeS;
    StateVector quaternionWxyz;
    double rawNormError;
  };

  std::vector<AttitudeSample> runAttitudeCase(const AttitudeCase &attitudeCase)
  {
    StateVector state = attitudeCase.initialQuaternionWxyz;
    const Eigen::Vector3d angularRateBodyRadps = attitudeCase.angularRateBodyRadps;

    const DerivativeFunction derivative = [angularRateBodyRadps](double timeS, const StateVector &evaluationState) -> StateVector { return attitudeKinematicsDerivative(timeS, evaluationState, angularRateBodyRadps); };

    std::vector<AttitudeSample> samples;
    samples.reserve(attitudeCase.numIntegrationSteps + 1U);
    samples.push_back({0.0, state, 0.0}); // 初态尚未经过 RK4 整步。

    for (std::size_t i = 0; i < attitudeCase.numIntegrationSteps; ++i)
    {
      const double timeS = static_cast<double>(i) * attitudeCase.dtS;
      const StateVector rawNextState = rk4Step(derivative, timeS, state, attitudeCase.dtS);
      const double rawNormError = std::abs(rawNextState.norm() - 1.0);

      const Eigen::Quaterniond rawQuaternion{rawNextState(0), rawNextState(1), rawNextState(2), rawNextState(3)};
      const Eigen::Quaterniond unitQuaternion = normalizeQuaternion(rawQuaternion);
      state << unitQuaternion.w(), unitQuaternion.x(), unitQuaternion.y(), unitQuaternion.z();

      const double nextTimeS = static_cast<double>(i + 1U) * attitudeCase.dtS;
      samples.push_back({nextTimeS, state, rawNormError});
    }
    return samples;
  }
} // namespace aerial_control

int main()
{
  try
  {
    aerial_control::StateVector initialQuaternionWxyz(4);
    initialQuaternionWxyz << 1.0, 0.0, 0.0, 0.0;
    const double pi = std::acos(-1.0);

    const aerial_control::AttitudeCase stationary{"stationary", initialQuaternionWxyz, Eigen::Vector3d::Zero(), 0.01, 200U};
    const aerial_control::AttitudeCase positiveYaw{"positive yaw", initialQuaternionWxyz, Eigen::Vector3d{0.0, 0.0, pi / 4.0}, 0.01, 200U};

    std::cout << std::setprecision(15);
    for (const aerial_control::AttitudeCase &attitudeCase : {stationary, positiveYaw})
    {
      const std::vector<aerial_control::AttitudeSample> samples = aerial_control::runAttitudeCase(attitudeCase);
      const aerial_control::AttitudeSample &finalSample = samples.back();
      const aerial_control::StateVector &finalQuaternionWxyz = finalSample.quaternionWxyz;
      const Eigen::Quaterniond finalQuaternion{finalQuaternionWxyz(0), finalQuaternionWxyz(1), finalQuaternionWxyz(2), finalQuaternionWxyz(3)};
      const Eigen::Vector3d bodyForwardNed = aerial_control::rotateBodyToNed(finalQuaternion, Eigen::Vector3d::UnitX());

      std::cout << "case: " << attitudeCase.name << '\n' << "integration steps: " << attitudeCase.numIntegrationSteps << '\n' << "samples: " << samples.size() << '\n' << "final time (s): " << finalSample.timeS << '\n' << "final q_NB [w x y z]: " << finalQuaternionWxyz.transpose() << '\n' << "body forward in NED [N E D]: " << bodyForwardNed.transpose() << '\n';
    }
  }
  catch (const std::exception &error)
  {
    std::cerr << "attitude_kinematics_demo: " << error.what() << '\n';
    return 1;
  }
  return 0;
}
