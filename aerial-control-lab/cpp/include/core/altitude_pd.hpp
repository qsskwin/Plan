#pragma once

namespace aerial_control {

struct AltitudePdParameters {
    double nominalMassKg;
    double kpPerSecondSquared;
    double kdPerSecond;
    double minThrustN; 
    double maxThrustN;
};

struct AltitudeCommand {
    double rawThrustN;
    double appliedThrustN;
};

class AltitudePdController {
 public:
    explicit AltitudePdController(AltitudePdParameters parameters);

    AltitudeCommand compute(
        double referenceHeightM,
        double heightM,
        double verticalSpeedMps) const;

 private:
    AltitudePdParameters parameters_;
};

}  // namespace aerial_control