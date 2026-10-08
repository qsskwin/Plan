#include "core/attitude_kinematics.hpp"
#include "core/integrators.hpp"
#include "core/rotation.hpp"

#include <Eigen/Geometry>

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

  struct AttitudeMetrics
  {
    double maxQuaternionError = 0.0;
    double maxRawNormError = 0.0;
    double maxNormalizedNormError = 0.0;
    double maxAxisDirectionError = 0.0;
  };

  std::vector<AttitudeSample> runAttitudeCase(const AttitudeCase &attitudeCase) //积分循环和四元数转换
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

      const Eigen::Quaterniond rawQuaternion{rawNextState(0), rawNextState(1), rawNextState(2), rawNextState(3)};  //四元数转换
      const Eigen::Quaterniond unitQuaternion = normalizeQuaternion(rawQuaternion);
      state << unitQuaternion.w(), unitQuaternion.x(), unitQuaternion.y(), unitQuaternion.z();

      const double nextTimeS = static_cast<double>(i + 1U) * attitudeCase.dtS;
      samples.push_back({nextTimeS, state, rawNormError});
    }
    return samples;
  }

  AttitudeMetrics summarizeAttitudeCase(const AttitudeCase &attitudeCase, const std::vector<AttitudeSample> &samples)
  {
    const StateVector &initial = attitudeCase.initialQuaternionWxyz;
    const Eigen::Quaterniond initialQuaternion{initial(0), initial(1), initial(2), initial(3)};
    const double rateNorm = attitudeCase.angularRateBodyRadps.norm();
    Eigen::Vector3d rotationAxis = Eigen::Vector3d::UnitX();
    if (rateNorm > 0.0)
    {
      rotationAxis = attitudeCase.angularRateBodyRadps / rateNorm;
    }

    AttitudeMetrics metrics;
    for (const AttitudeSample &sample : samples)
    {
      const StateVector &state = sample.quaternionWxyz;
      const Eigen::Quaterniond numerical{state(0), state(1), state(2), state(3)};
      const Eigen::AngleAxisd relativeRotation{rateNorm * sample.timeS, rotationAxis};
      const Eigen::Quaterniond reference = initialQuaternion * Eigen::Quaterniond{relativeRotation};
      const Eigen::Matrix3d referenceMatrix = initialQuaternion.toRotationMatrix() * relativeRotation.toRotationMatrix();
      const Eigen::Vector4d numericalWxyz{numerical.w(), numerical.x(), numerical.y(), numerical.z()};
      const Eigen::Vector4d referenceWxyz{reference.w(), reference.x(), reference.y(), reference.z()};

      metrics.maxQuaternionError = std::max(metrics.maxQuaternionError,
                                            std::min((numericalWxyz - referenceWxyz).norm(), (numericalWxyz + referenceWxyz).norm()));
      metrics.maxRawNormError = std::max(metrics.maxRawNormError, sample.rawNormError);
      metrics.maxNormalizedNormError = std::max(metrics.maxNormalizedNormError, std::abs(state.norm() - 1.0));

      for (int axisIndex = 0; axisIndex < 3; ++axisIndex)
      {
        Eigen::Vector3d bodyAxis = Eigen::Vector3d::Zero();
        bodyAxis(axisIndex) = 1.0;
        const Eigen::Vector3d numericalAxis = rotateBodyToNed(numerical, bodyAxis);
        metrics.maxAxisDirectionError = std::max(metrics.maxAxisDirectionError,
                                                 (numericalAxis - referenceMatrix.col(axisIndex)).norm());
      }
    }
    return metrics;
  }
} // namespace aerial_control

int main(int argc, char *argv[])
{
  try
  {
    const bool exportTrajectoryCsv = argc == 2 && std::string(argv[1]) == "--trajectory-csv";
    if (argc != 1 && !exportTrajectoryCsv)
    {
      throw std::invalid_argument("usage: attitude_kinematics_demo [--trajectory-csv]");
    }

    aerial_control::StateVector initialQuaternionWxyz(4), initialYaw90QuaternionWxyz(4);
    initialQuaternionWxyz << 1.0, 0.0, 0.0, 0.0;
    initialYaw90QuaternionWxyz << 0.7071067811865476, 0.0, 0.0, 0.7071067811865475;
    const double pi = std::acos(-1.0);

    const aerial_control::AttitudeCase stationary{"stationary", initialQuaternionWxyz, Eigen::Vector3d::Zero(), 0.01, 200U};
    const aerial_control::AttitudeCase positiveYaw{"positive yaw", initialQuaternionWxyz, Eigen::Vector3d{0.0, 0.0, pi / 4.0}, 0.01, 200U};
    const aerial_control::AttitudeCase positiveRoll{"positive roll", initialQuaternionWxyz, Eigen::Vector3d{pi / 36.0, 0.0, 0.0}, 0.01, 200U};
    const aerial_control::AttitudeCase initialYaw90Quaternion{"initial yaw 90 then body roll 90", initialYaw90QuaternionWxyz, Eigen::Vector3d{pi / 4.0, 0.0, 0.0}, 0.01, 200U};
    std::cout << std::setprecision(exportTrajectoryCsv ? 17 : 15);
    if (exportTrajectoryCsv)
    {
      std::cout << "case,time_s,qw,qx,qy,qz,raw_norm_error,x_N,x_E,x_D,y_N,y_E,y_D,z_N,z_E,z_D\n";
    }

    for (const aerial_control::AttitudeCase &attitudeCase : {stationary, positiveYaw, positiveRoll, initialYaw90Quaternion})
    {
      const std::vector<aerial_control::AttitudeSample> samples = aerial_control::runAttitudeCase(attitudeCase);
      if (exportTrajectoryCsv)
      {
        for (const aerial_control::AttitudeSample &sample : samples)
        {
          const aerial_control::StateVector &state = sample.quaternionWxyz;
          const Eigen::Quaterniond quaternion{state(0), state(1), state(2), state(3)};
          std::cout << attitudeCase.name << ',' << sample.timeS << ',' << state(0) << ',' << state(1) << ',' << state(2) << ',' << state(3) << ',' << sample.rawNormError;
          for (int axisIndex = 0; axisIndex < 3; ++axisIndex)
          {
            Eigen::Vector3d bodyAxis = Eigen::Vector3d::Zero();
            bodyAxis(axisIndex) = 1.0;
            const Eigen::Vector3d axisNed = aerial_control::rotateBodyToNed(quaternion, bodyAxis);
            std::cout << ',' << axisNed(0) << ',' << axisNed(1) << ',' << axisNed(2);
          }
          std::cout << '\n';
        }
        continue;
      }

      const aerial_control::AttitudeMetrics metrics = aerial_control::summarizeAttitudeCase(attitudeCase, samples);
      const aerial_control::AttitudeSample &finalSample = samples.back();
      const aerial_control::StateVector &finalQuaternionWxyz = finalSample.quaternionWxyz;
      const Eigen::Quaterniond finalQuaternion{finalQuaternionWxyz(0), finalQuaternionWxyz(1), finalQuaternionWxyz(2), finalQuaternionWxyz(3)};
      const Eigen::Vector3d bodyForwardNed = aerial_control::rotateBodyToNed(finalQuaternion, Eigen::Vector3d::UnitX());
      const Eigen::Vector3d bodyUpNed = aerial_control::rotateBodyToNed(finalQuaternion, -Eigen::Vector3d::UnitZ());
      std::cout << "case: " << attitudeCase.name << '\n' << "integration steps: " << attitudeCase.numIntegrationSteps << '\n' << "samples: " << samples.size() << '\n' << "final time (s): " << finalSample.timeS << '\n' << "final q_NB [w x y z]: " << finalQuaternionWxyz.transpose() << '\n' << "body forward in NED [N E D]: " << bodyForwardNed.transpose() << '\n';
      std::cout << "body up in NED [N E D]: " << bodyUpNed.transpose() << '\n';
      std::cout << "max quaternion error (sign invariant): " << metrics.maxQuaternionError << '\n';
      std::cout << "max axis direction error: " << metrics.maxAxisDirectionError << '\n';
      std::cout << "max pre-normalization norm error: " << metrics.maxRawNormError << '\n';
      std::cout << "max post-normalization norm error: " << metrics.maxNormalizedNormError << '\n';
    }
  }
  catch (const std::exception &error)
  {
    std::cerr << "attitude_kinematics_demo: " << error.what() << '\n';
    return 1;
  }
  return 0;
}
