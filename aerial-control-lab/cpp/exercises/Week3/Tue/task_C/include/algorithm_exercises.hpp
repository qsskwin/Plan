#pragma once

#include <cstddef>
#include <vector>

namespace aerial_control::week3_tue {
void sort_ascending(std::vector<double>& values);
bool contains_value(const std::vector<double>& values, double target);
bool has_value_above(const std::vector<double>& values, double threshold);
std::size_t count_value(const std::vector<double>& values, double target);
std::size_t count_values_above(const std::vector<double>& values,
                               double threshold);
std::vector<double> scaled_copy(const std::vector<double>& values,
                                double scale);
void erase_negative_values(std::vector<double>& values);
}  // namespace aerial_control::week3_tue
