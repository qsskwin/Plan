#include "algorithm_exercises.hpp"

#include <algorithm>
#include <vector>

namespace aerial_control::week3_tue {
void sort_ascending(std::vector<double>& values) {
  std::sort(values.begin(), values.end());
}

bool contains_value(const std::vector<double>& values, double target) {
  return std::find(values.begin(), values.end(), target) != values.end();
}

bool has_value_above(const std::vector<double>& values, double threshold) {
  const auto it = std::find_if(values.begin(), values.end(),
                               [threshold](double v) { return v > threshold; });
  return it != values.end();
}

std::size_t count_value(const std::vector<double>& values, double target) {
  return static_cast<std::size_t>(
      std::count(values.begin(), values.end(), target));
}

std::size_t count_values_above(const std::vector<double>& values,
                               double threshold) {
  return static_cast<std::size_t>(
      std::count_if(values.begin(), values.end(),
                    [threshold](double v) { return v > threshold; }));
}

std::vector<double> scaled_copy(const std::vector<double>& values,
                                double scale) {
  std::vector<double> result(values.size());
  std::transform(values.begin(), values.end(), result.begin(),
                 [scale](double value) { return value * scale; });
  return result;
}

void erase_negative_values(std::vector<double>& values) {
  const auto new_end = std::remove_if(values.begin(), values.end(),
                                      [](double value) { return value < 0.0; });
  values.erase(new_end, values.end());
}
}  // namespace aerial_control::week3_tue