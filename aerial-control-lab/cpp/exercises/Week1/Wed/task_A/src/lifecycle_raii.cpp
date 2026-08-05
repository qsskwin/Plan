#include "week1_wed_task_a/lifecycle_raii.hpp"

#include <chrono>
#include <memory>
#include <ostream>
#include <utility>

namespace week1_wed_task_a {

LifecycleProbe::LifecycleProbe(LifecycleStats& stats, const int value) noexcept
    : stats_(stats), value_(value) {
    ++stats_.direct_constructions;
}

LifecycleProbe::~LifecycleProbe() noexcept {
    ++stats_.destructions;
}

LifecycleProbe::LifecycleProbe(const LifecycleProbe& other) noexcept
    : stats_(other.stats_), value_(other.value_) {
    ++stats_.copy_constructions;
}

LifecycleProbe::LifecycleProbe(LifecycleProbe&& other) noexcept
    : stats_(other.stats_), value_(other.value_) {
    ++stats_.move_constructions;
    other.value_ = 0;
}

int LifecycleProbe::value() const noexcept {
    return value_;
}

ScopeTimer::ScopeTimer(std::string label, std::ostream& output)
    : label_(std::move(label)), output_(&output), start_(Clock::now()) {}

ScopeTimer::~ScopeTimer() noexcept {
    const auto elapsed =
        std::chrono::duration_cast<std::chrono::microseconds>(Clock::now() - start_);

    // 析构函数不能把输出异常传播出作用域，否则可能触发 std::terminate。
    try {
        *output_ << "[ScopeTimer] " << label_ << ": " << elapsed.count() << " us\n";
    } catch (...) {
        // 计时信息属于辅助诊断；输出失败不应改变被计时代码的控制流。
    }
}

std::unique_ptr<LifecycleProbe> make_owned_probe(LifecycleStats& stats, const int value) {
    return std::make_unique<LifecycleProbe>(stats, value);
}

}  // namespace week1_wed_task_a
