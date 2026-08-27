#include "sample_buffer.hpp"

namespace aerial_control::week3_mon {

void SampleBuffer::add(double sample) {
    samples_.push_back(sample);
}

bool SampleBuffer::empty() const noexcept {
    return samples_.empty();
}

std::size_t SampleBuffer::size() const noexcept {
    return samples_.size();
}

double SampleBuffer::at(std::size_t index) const {
    return samples_.at(index);
}

}  // namespace aerial_control::week3_mon