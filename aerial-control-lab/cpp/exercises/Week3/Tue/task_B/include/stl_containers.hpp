#pragma once

#include <array>
#include <cstddef>
#include <deque>
#include <string>
#include <unordered_map>
#include <vector>

namespace aerial_control::week3_tue {

using ThreeAxisState = std::array<double, 3>;
using StateCounts = std::unordered_map<std::string, std::size_t>;

ThreeAxisState make_three_axis_state(double x, double y, double z);
double mean(const std::vector<double>& samples);
void push_rolling_sample(std::deque<double>& buffer, std::size_t max_capacity,
                         double sample);
StateCounts count_states(const std::vector<std::string>& states);
std::vector<double> largest_errors(const std::vector<double>& errors,
                                   std::size_t count);
}  // namespace aerial_control::week3_tue