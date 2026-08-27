#pragma once

#include <cstddef>
#include <vector>

namespace aerial_control::week3_mon {

class SampleBuffer {
 public:
  SampleBuffer() = default;

  void add(double sample);

  [[nodiscard]] bool empty() const noexcept;
  [[nodiscard]] std::size_t size() const noexcept;
  [[nodiscard]] double at(std::size_t index) const;

 private:
  std::vector<double> samples_;
};

}  // namespace aerial_control::week3_mon