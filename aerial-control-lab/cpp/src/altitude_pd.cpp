#include "core/altitude_pd.hpp"
#include "core/point_mass.hpp"

#include <cmath>
#include <stdexcept>
#include <algorithm>

namespace aerial_control {

AltitudePdController::AltitudePdController(AltitudePdParameters parameters)
    : parameters_(parameters) {
  if (!std::isfinite(parameters_.nominalMassKg) ||
      parameters_.nominalMassKg <= 0.0) {
    throw std::invalid_argument(
        "AltitudePdController: nominal mass must be positive and finite");
  }
  if (!std::isfinite(parameters_.kpPerSecondSquared) ||
      parameters_.kpPerSecondSquared < 0.0) {
    throw std::invalid_argument(
        "AltitudePdController: kp must be non-negative and finite");
  }
  if (!std::isfinite(parameters_.kdPerSecond) ||
      parameters_.kdPerSecond < 0.0) {
    throw std::invalid_argument(
        "AltitudePdController: kd must be non-negative and finite");
  }
  if (!std::isfinite(parameters_.minThrustN) || parameters_.minThrustN < 0.0) {
    throw std::invalid_argument(
        "AltitudePdController: min thrust must be non-negative and finite");
  }
  if (!std::isfinite(parameters_.maxThrustN) ||
      parameters_.maxThrustN < parameters_.minThrustN) {
    throw std::invalid_argument(
        "AltitudePdController: max thrust must be >= min thrust and finite");
  }
}

AltitudeCommand AltitudePdController::compute(
    double referenceHeightM,
    double heightM,
    double verticalSpeedMps) const {
    
    if(!std::isfinite(referenceHeightM) || !std::isfinite(heightM) || !std::isfinite(verticalSpeedMps)) {
        throw std::invalid_argument("AltitudePdController: inputs must be finite");
    }
    double heightErrorM = referenceHeightM - heightM;
    double accelerationCommandMps2 = parameters_.kpPerSecondSquared * heightErrorM - parameters_.kdPerSecond * verticalSpeedMps;
    AltitudeCommand command;
    command.rawThrustN = parameters_.nominalMassKg * (accelerationCommandMps2 + kStandardGravityMps2);
    command.appliedThrustN = std::clamp(command.rawThrustN, parameters_.minThrustN, parameters_.maxThrustN);
    return command;

    
}


}  // namespace aerial_control