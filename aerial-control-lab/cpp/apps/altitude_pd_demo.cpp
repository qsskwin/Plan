#include <Eigen/Geometry>
#include <cstddef>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>

#include "core/altitude_pd.hpp"
#include "core/integrators.hpp"
#include "core/point_mass.hpp"

namespace
{

  struct Sample
  {
    double timeS;
    double referenceHeightM;
    double heightM;
    double verticalSpeedMps;
    double rawThrustN;
    double appliedThrustN;
  };

  struct CaseDefinition
  {
    std::string name;
    double actualMassKg;
    double initialHeightM;
    double initialReferenceHeightM;
    double stepReferenceHeightM;
    double stepTimeS;
    double dtS;
    std::size_t numIntegrationSteps;
    std::size_t controlStride;
  };

  struct CaseResult
  {
    aerial_control::StateVector finalState;
    std::size_t integrationCount;
    std::size_t controlUpdateCount;
    std::vector<Sample> samples;
  };

  struct CaseSummary
  {
    double finalTimeS;
    double finalHeightM;
    double finalVerticalSpeedMps;
    std::size_t lastSecondSampleCount;
    double lastSecondAverageHeightErrorM;
  };

  double referenceHeightAtTime(const CaseDefinition &caseDefinition, const double timeS) { return timeS < caseDefinition.stepTimeS ? caseDefinition.initialReferenceHeightM : caseDefinition.stepReferenceHeightM; }

  CaseResult runCase(const CaseDefinition &caseDefinition, const aerial_control::AltitudePdController &controller)
  {
    if (caseDefinition.controlStride == 0U)
    {
      throw std::invalid_argument("control stride must be positive");
    }

    const aerial_control::PointMassParameters plantParameters{
        caseDefinition.actualMassKg,
    };
    aerial_control::PointMassInput heldInput{
        0.0,
        Eigen::Quaterniond{1.0, 0.0, 0.0, 0.0},
    };

    CaseResult result{
        aerial_control::StateVector::Zero(6),
        0U,
        0U,
        {},
    };
    result.finalState(2) = -caseDefinition.initialHeightM;
    result.samples.reserve(caseDefinition.numIntegrationSteps / caseDefinition.controlStride);

    const aerial_control::DerivativeFunction dynamics = [&plantParameters, &heldInput](const double timeS, const aerial_control::StateVector &evaluationState) { return aerial_control::pointMassDerivative(timeS, evaluationState, plantParameters, heldInput); };

    for (std::size_t step = 0U; step < caseDefinition.numIntegrationSteps; ++step)
    {
      const double timeS = static_cast<double>(step) * caseDefinition.dtS;

      if (step % caseDefinition.controlStride == 0U)
      {
        const double referenceHeightM = referenceHeightAtTime(caseDefinition, timeS);
        const double heightM = -result.finalState(2); // 在这里进行更新
        const double verticalSpeedMps = -result.finalState(5);
        const aerial_control::AltitudeCommand command = controller.compute(referenceHeightM, heightM, verticalSpeedMps);

        heldInput.totalThrustN = command.appliedThrustN;
        result.samples.push_back(Sample{
            timeS,
            referenceHeightM,
            heightM,
            verticalSpeedMps,
            command.rawThrustN,
            command.appliedThrustN,
        });
        ++result.controlUpdateCount;
      }

      result.finalState = aerial_control::rk4Step(dynamics, timeS, result.finalState, caseDefinition.dtS);
      ++result.integrationCount;
    }

    return result;
  }

  CaseSummary summarizeCase(const CaseDefinition &caseDefinition, const CaseResult &result)
  {
    const double finalTimeS = static_cast<double>(caseDefinition.numIntegrationSteps) * caseDefinition.dtS;
    const double lastSecondStartTimeS = finalTimeS - 1.0;
    double lastSecondHeightErrorSumM = 0.0;
    std::size_t lastSecondSampleCount = 0U;

    for (const Sample &sample : result.samples)
    {
      if (sample.timeS >= lastSecondStartTimeS)
      {
        lastSecondHeightErrorSumM += sample.referenceHeightM - sample.heightM;
        ++lastSecondSampleCount;
      }
    }
    if (lastSecondSampleCount == 0U)
    {
      throw std::runtime_error("no samples found in the last second");
    }

    return {
        finalTimeS, -result.finalState(2), -result.finalState(5), lastSecondSampleCount, lastSecondHeightErrorSumM / static_cast<double>(lastSecondSampleCount),
    };
  }

  void printSummary(const CaseDefinition &caseDefinition, const CaseResult &result, const CaseSummary &summary)
  {
    std::cout << "case: " << caseDefinition.name << '\n';
    std::cout << "actual mass: " << caseDefinition.actualMassKg << " kg\n";
    std::cout << "integration count: " << result.integrationCount << '\n';
    std::cout << "control update count: " << result.controlUpdateCount << '\n';
    std::cout << "sample count: " << result.samples.size() << '\n';
    std::cout << "sample capacity: " << result.samples.capacity() << '\n';
    std::cout << "final time: " << summary.finalTimeS << " s\n";
    std::cout << "final height: " << summary.finalHeightM << " m\n";
    std::cout << "final vertical speed: " << summary.finalVerticalSpeedMps << " m/s\n";
    std::cout << "final north position: " << result.finalState(0) << " m\n";
    std::cout << "final east position: " << result.finalState(1) << " m\n";
    std::cout << "final north velocity: " << result.finalState(3) << " m/s\n";
    std::cout << "final east velocity: " << result.finalState(4) << " m/s\n";
    std::cout << "last-second sample count: " << summary.lastSecondSampleCount << '\n';
    std::cout << "last-second average height error: " << summary.lastSecondAverageHeightErrorM << " m\n";
  }

  void printSaturationChecks(const aerial_control::AltitudePdController &controller)
  {
    const aerial_control::AltitudeCommand upperLimitCommand = controller.compute(10.0, 0.0, 0.0);
    const aerial_control::AltitudeCommand lowerLimitCommand = controller.compute(0.0, 10.0, 0.0);

    std::cout << "upper-limit check: raw_thrust=" << upperLimitCommand.rawThrustN << " N, applied_thrust=" << upperLimitCommand.appliedThrustN << " N\n";
    std::cout << "lower-limit check: raw_thrust=" << lowerLimitCommand.rawThrustN << " N, applied_thrust=" << lowerLimitCommand.appliedThrustN << " N\n";
  }

  void exportSamplesCsv(const std::string &outputPath, const std::vector<Sample> &samples)
  {
    std::ofstream output{outputPath};
    if (!output)
    {
      throw std::runtime_error("failed to open CSV output: " + outputPath);
    }

    output << "time_s,reference_height_m,height_m,vertical_speed_mps,"
              "raw_thrust_n,applied_thrust_n\n";
    output << std::setprecision(std::numeric_limits<double>::max_digits10);
    for (const Sample &sample : samples)
    {
      output << sample.timeS << ',' << sample.referenceHeightM << ',' << sample.heightM << ',' << sample.verticalSpeedMps << ',' << sample.rawThrustN << ',' << sample.appliedThrustN << '\n';
    }
  }

} // namespace

int main(int argc, char *argv[])
{
  try
  {
    std::string outputCsvPath;
    for (int index = 1; index < argc; ++index)
    {
      const std::string argument{argv[index]};
      if (argument == "--output-csv")
      {
        if (index + 1 >= argc)
        {
          throw std::invalid_argument("--output-csv requires a file path");
        }
        outputCsvPath = argv[++index];
      }
      else
      {
        throw std::invalid_argument("unknown argument: " + argument);
      }
    }

    const aerial_control::AltitudePdParameters controllerParameters{
        1.5,      // nominalMassKg
        4.0,      // kpPerSecondSquared
        4.0,      // kdPerSecond
        0.0,      // minThrustN
        29.41995, // maxThrustN
    };
    const aerial_control::AltitudePdController controller{
        controllerParameters,
    };
    const CaseDefinition massBiasCase{
        "10% actual-mass increase",
        1.65,  // actualMassKg
        1.0,   // initialHeightM
        1.0,   // initialReferenceHeightM
        1.0,   // stepReferenceHeightM
        1.0,   // stepTimeS
        0.01,  // dtS
        1000U, // numIntegrationSteps
        2U,    // controlStride
    };

    const CaseResult result = runCase(massBiasCase, controller);
    const CaseSummary summary = summarizeCase(massBiasCase, result);

    std::cout << std::setprecision(15);
    printSummary(massBiasCase, result, summary);
    printSaturationChecks(controller);

    if (!outputCsvPath.empty())
    {
      exportSamplesCsv(outputCsvPath, result.samples);
      std::cout << "sample CSV: " << outputCsvPath << '\n';
    }
  }
  catch (const std::exception &error)
  {
    std::cerr << "altitude_pd_demo: " << error.what() << '\n';
    return 1;
  }
  return 0;
}
