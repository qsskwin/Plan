#include "core/integrators.hpp"
#include "core/point_mass.hpp"

#include <Eigen/Core>
#include <Eigen/Geometry>

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

constexpr double kMassKg = 1.5;
constexpr double kMassBiasKg = 1.65;
constexpr double kT0S = 0.0;
constexpr double kDtS = 0.01;
constexpr double kPositionAtolM = 1.0e-10;
constexpr double kVelocityAtolMps = 1.0e-10;
constexpr double kPi = 3.141592653589793238462643383279502884;
constexpr double kRollRad = 10.0 * kPi / 180.0;

struct CaseData {
    std::string slug;
    std::string displayName;
    double massKg;
    double totalThrustN;
    Eigen::Quaterniond qNb;
    double t0S;
    double tfS;
    double dtS;
};

struct CaseResult {
    std::vector<double> times;
    std::vector<aerial_control::StateVector> numericalStates;
    std::vector<aerial_control::StateVector> analyticalStates;
    double maxPositionErrorM;
    double maxVelocityErrorMps;
};

std::vector<CaseData> buildCases() {
    const Eigen::Quaterniond identity_q_nb{1.0, 0.0, 0.0, 0.0};
    const Eigen::Quaterniond roll_q_nb{
        std::cos(kRollRad / 2.0),
        std::sin(kRollRad / 2.0),
        0.0,
        0.0,
    };
    const double hover_thrust_n =
        kMassKg * aerial_control::kStandardGravityMps2;

    return {
        {
            "free_fall",
            "Free fall",
            kMassKg,
            0.0,
            identity_q_nb,
            kT0S,
            2.0,
            kDtS,
        },
        {
            "hover",
            "Level hover",
            kMassKg,
            hover_thrust_n,
            identity_q_nb,
            kT0S,
            10.0,
            kDtS,
        },
        {
            "positive_roll",
            "Fixed positive roll",
            kMassKg,
            hover_thrust_n / std::cos(kRollRad),
            roll_q_nb,
            kT0S,
            2.0,
            kDtS,
        },
    };
}

CaseData buildMassBiasCase() {
    return {
        "mass_bias",
        "Level attitude with 10% mass increase",
        kMassBiasKg,
        kMassKg * aerial_control::kStandardGravityMps2,
        Eigen::Quaterniond{1.0, 0.0, 0.0, 0.0},
        kT0S,
        2.0,
        kDtS,
    };
}

aerial_control::StateVector analyticalState(
    const CaseData& case_data,
    const double time_s) {
    aerial_control::StateVector state =
        aerial_control::StateVector::Zero(6);
    const double elapsed_s = time_s - case_data.t0S;

    if (case_data.slug == "free_fall") {
        state(2) = 0.5 * aerial_control::kStandardGravityMps2 *
                   elapsed_s * elapsed_s;
        state(5) = aerial_control::kStandardGravityMps2 * elapsed_s;
    } else if (case_data.slug == "hover") {
        return state;
    } else if (case_data.slug == "positive_roll") {
        const double east_acceleration_mps2 =
            aerial_control::kStandardGravityMps2 * std::tan(kRollRad);
        state(1) = 0.5 * east_acceleration_mps2 * elapsed_s * elapsed_s;
        state(4) = east_acceleration_mps2 * elapsed_s;
    } else if (case_data.slug == "mass_bias") {
        const double down_acceleration_mps2 =
            aerial_control::kStandardGravityMps2 -
            case_data.totalThrustN / case_data.massKg;
        state(2) = 0.5 * down_acceleration_mps2 * elapsed_s * elapsed_s;
        state(5) = down_acceleration_mps2 * elapsed_s;
    } else {
        throw std::invalid_argument(
            "Analytical solution is not implemented for " + case_data.slug);
    }

    return state;
}

CaseResult runCase(const CaseData& case_data) {
    const double duration_s = case_data.tfS - case_data.t0S;
    const double step_ratio = duration_s / case_data.dtS;
    const auto num_steps = static_cast<std::size_t>(std::llround(step_ratio));
    if (std::abs(static_cast<double>(num_steps) * case_data.dtS - duration_s) >
        1.0e-12) {
        throw std::invalid_argument("The time interval must be divisible by dt");
    }

    const aerial_control::PointMassParameters parameters{case_data.massKg};
    const aerial_control::PointMassInput input{
        case_data.totalThrustN,
        case_data.qNb,
    };
    const aerial_control::DerivativeFunction dynamics =
        [&parameters, &input](
            const double time_s,
            const aerial_control::StateVector& state) {
            return aerial_control::pointMassDerivative(
                time_s, state, parameters, input);
        };

    CaseResult result;
    result.times.reserve(num_steps + 1U);
    result.numericalStates.reserve(num_steps + 1U);
    result.analyticalStates.reserve(num_steps + 1U);

    result.times.push_back(case_data.t0S);
    result.numericalStates.push_back(
        aerial_control::StateVector::Zero(6));

    for (std::size_t step = 0; step < num_steps; ++step) {
        const double time_s =
            case_data.t0S + static_cast<double>(step) * case_data.dtS;
        result.numericalStates.push_back(aerial_control::rk4Step(
            dynamics,
            time_s,
            result.numericalStates.back(),
            case_data.dtS));

        double next_time_s = case_data.t0S +
                             static_cast<double>(step + 1U) * case_data.dtS;
        if (step + 1U == num_steps) {
            next_time_s = case_data.tfS;
        }
        result.times.push_back(next_time_s);
    }

    result.maxPositionErrorM = 0.0;
    result.maxVelocityErrorMps = 0.0;
    for (std::size_t index = 0; index < result.times.size(); ++index) {
        const aerial_control::StateVector analytical =
            analyticalState(case_data, result.times[index]);
        const aerial_control::StateVector absolute_error =
            (result.numericalStates[index] - analytical).cwiseAbs();
        result.maxPositionErrorM = std::max(
            result.maxPositionErrorM,
            absolute_error.head<3>().maxCoeff());
        result.maxVelocityErrorMps = std::max(
            result.maxVelocityErrorMps,
            absolute_error.tail<3>().maxCoeff());
        result.analyticalStates.push_back(analytical);
    }

    if (result.maxPositionErrorM > kPositionAtolM) {
        throw std::runtime_error(
            case_data.slug + " exceeded the position tolerance");
    }
    if (result.maxVelocityErrorMps > kVelocityAtolMps) {
        throw std::runtime_error(
            case_data.slug + " exceeded the velocity tolerance");
    }
    return result;
}

std::filesystem::path saveTrajectory(
    const std::filesystem::path& output_dir,
    const CaseData& case_data,
    const CaseResult& result) {
    std::filesystem::create_directories(output_dir);
    const std::filesystem::path output_path =
        output_dir / ("cpp_" + case_data.slug + ".csv");
    std::ofstream output{output_path};
    if (!output) {
        throw std::runtime_error(
            "Failed to open trajectory output: " + output_path.string());
    }

    output << "time_s,num_p_n_m,num_p_e_m,num_p_d_m,num_v_n_mps,"
              "num_v_e_mps,num_v_d_mps,ana_p_n_m,ana_p_e_m,ana_p_d_m,"
              "ana_v_n_mps,ana_v_e_mps,ana_v_d_mps,abs_err_p_n_m,"
              "abs_err_p_e_m,abs_err_p_d_m,abs_err_v_n_mps,"
              "abs_err_v_e_mps,abs_err_v_d_mps\n";
    output << std::setprecision(std::numeric_limits<double>::max_digits10);

    for (std::size_t index = 0; index < result.times.size(); ++index) {
        const auto& numerical = result.numericalStates[index];
        const auto& analytical = result.analyticalStates[index];
        const auto absolute_error = (numerical - analytical).cwiseAbs();

        output << result.times[index];
        for (Eigen::Index component = 0; component < numerical.size();
             ++component) {
            output << ',' << numerical(component);
        }
        for (Eigen::Index component = 0; component < analytical.size();
             ++component) {
            output << ',' << analytical(component);
        }
        for (Eigen::Index component = 0; component < 3; ++component) {
            output << ',' << absolute_error(component);
        }
        for (Eigen::Index component = 3; component < 6; ++component) {
            output << ',' << absolute_error(component);
        }
        output << '\n';
    }
    return output_path;
}

void printState(const aerial_control::StateVector& state) {
    std::cout << '[';
    for (Eigen::Index component = 0; component < state.size(); ++component) {
        if (component != 0) {
            std::cout << ", ";
        }
        std::cout << state(component);
    }
    std::cout << ']';
}

void printSummary(const CaseData& case_data, const CaseResult& result) {
    std::cout << "Case: " << case_data.displayName << " (" << case_data.slug
              << ")\n";
    std::cout << "Parameters: mass=" << case_data.massKg
              << " kg, thrust=" << case_data.totalThrustN
              << " N, q_nb=[" << case_data.qNb.w() << ", "
              << case_data.qNb.x() << ", " << case_data.qNb.y() << ", "
              << case_data.qNb.z() << "], dt=" << case_data.dtS << " s\n";
    std::cout << "Final time: " << result.times.back() << " s\n";
    std::cout << "Numerical final state [p_n,p_e,p_d,v_n,v_e,v_d]: ";
    printState(result.numericalStates.back());
    std::cout << '\n';
    std::cout << "Analytical final state [p_n,p_e,p_d,v_n,v_e,v_d]: ";
    printState(result.analyticalStates.back());
    std::cout << '\n';
    std::cout << "Maximum position error: " << result.maxPositionErrorM
              << " m\n";
    std::cout << "Maximum velocity error: " << result.maxVelocityErrorMps
              << " m/s\n\n";
}

}  // namespace

int main(int argc, char* argv[]) {
    try {
        std::filesystem::path output_dir =
            "python/exercises/Week4/Fri/task_A/outputs";
        bool include_mass_bias = false;
        bool mass_bias_only = false;
        for (int index = 1; index < argc; ++index) {
            const std::string argument{argv[index]};
            if (argument == "--include-mass-bias") {
                include_mass_bias = true;
            } else if (argument == "--mass-bias-only") {
                mass_bias_only = true;
            } else if (argument == "--output-dir") {
                if (index + 1 >= argc) {
                    throw std::invalid_argument(
                        "--output-dir requires a path argument");
                }
                output_dir = argv[++index];
            } else {
                throw std::invalid_argument("Unknown argument: " + argument);
            }
        }
        if (include_mass_bias && mass_bias_only) {
            throw std::invalid_argument(
                "--include-mass-bias and --mass-bias-only are mutually exclusive");
        }

        std::cout << std::setprecision(
            std::numeric_limits<double>::max_digits10);
        std::cout << "Coordinate convention: NED world and FRD body; +z is "
                     "Down.\n";
        std::cout << "q_NB maps body B to NED N; Eigen::Quaterniond is "
                     "constructed as (w,x,y,z).\n";
        std::cout << "Thrust uses T >= 0 with FRD force vector [0,0,-T].\n";
        std::cout << "Integrator: aerial_control::rk4Step (classical fixed-step "
                     "RK4).\n";
        std::cout << "All trajectories use double precision and integer step "
                     "indexing.\n\n";

        std::vector<CaseData> cases =
            mass_bias_only ? std::vector<CaseData>{} : buildCases();
        if (include_mass_bias || mass_bias_only) {
            cases.push_back(buildMassBiasCase());
        }
        for (const CaseData& case_data : cases) {
            const CaseResult result = runCase(case_data);
            const std::filesystem::path output_path =
                saveTrajectory(output_dir, case_data, result);
            printSummary(case_data, result);
            std::cout << "Trajectory CSV: " << output_path.string() << "\n\n";
        }
    } catch (const std::exception& error) {
        std::cerr << "point_mass_demo: " << error.what() << '\n';
        return 1;
    }
    return 0;
}
