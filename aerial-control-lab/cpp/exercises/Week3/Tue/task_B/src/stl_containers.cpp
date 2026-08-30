#include "stl_containers.hpp"

#include <numeric>
#include <queue>
#include <stdexcept>

namespace aerial_control::week3_tue {
ThreeAxisState make_three_axis_state(double x, double y, double z) {
  return {x, y, z};
}

double mean(const std::vector<double>& samples) {
  if (samples.empty()) {
    throw std::invalid_argument("samples must not be empty");
  }

  const double total = std::accumulate(samples.begin(), samples.end(), 0.0);
  return total / static_cast<double>(samples.size());
}

void push_rolling_sample(std::deque<double>& buffer, std::size_t max_capacity,
                         double sample) {
  buffer.push_back(sample);
  while (buffer.size() > max_capacity) {
    buffer.pop_front();
  }
}

StateCounts count_states(const std::vector<std::string>& states) {
  StateCounts counts;
  for (const auto& state : states) {
    ++counts[state];
  }
  return counts;
}

std::vector<double> largest_errors(const std::vector<double>& errors,
                                   std::size_t count) {
  if (count > errors.size()) {
    throw std::invalid_argument(
        "count must not be greater than the number of errors");
  }

  if (count == 0) {
    return {};
  }

  std::priority_queue<double> error_queue;
  for (const auto& error : errors) {
    error_queue.push(error);
  }

  std::vector<double> largest_errors;
  largest_errors.reserve(count);

  for (std::size_t i = 0; i < count; ++i) {
    largest_errors.push_back(error_queue.top());
    error_queue.pop();
  }

  return largest_errors;
}

}  // namespace aerial_control::week3_tue
