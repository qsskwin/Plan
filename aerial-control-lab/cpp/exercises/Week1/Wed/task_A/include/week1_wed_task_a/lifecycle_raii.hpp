#pragma once

#include <chrono>
#include <cstddef>
#include <iosfwd>
#include <memory>
#include <string>

namespace week1_wed_task_a {

// 汇总 LifecycleProbe 各类生命周期事件。计数器由调用者拥有，且必须比所有
// 引用它的 LifecycleProbe 活得更久。
struct LifecycleStats {
    std::size_t direct_constructions{0};
    std::size_t copy_constructions{0};
    std::size_t move_constructions{0};
    std::size_t destructions{0};
};

// 用计数器观察构造、拷贝构造、移动构造和析构发生的时机。
class LifecycleProbe {
public:
    LifecycleProbe(LifecycleStats& stats, int value) noexcept;
    ~LifecycleProbe() noexcept;

    LifecycleProbe(const LifecycleProbe& other) noexcept;
    LifecycleProbe(LifecycleProbe&& other) noexcept;

    LifecycleProbe& operator=(const LifecycleProbe&) = delete;
    LifecycleProbe& operator=(LifecycleProbe&&) = delete;

    [[nodiscard]] int value() const noexcept;

private:
    LifecycleStats& stats_;
    int value_;
};

// 作用域计时器：构造时记录起点，析构时把持续时间写入调用者提供的输出流。
// 输出流由调用者拥有，必须覆盖 ScopeTimer 的整个生命周期。
class ScopeTimer {
public:
    ScopeTimer(std::string label, std::ostream& output);
    ~ScopeTimer() noexcept;

    ScopeTimer(const ScopeTimer&) = delete;
    ScopeTimer& operator=(const ScopeTimer&) = delete;
    ScopeTimer(ScopeTimer&&) = delete;
    ScopeTimer& operator=(ScopeTimer&&) = delete;

private:
    using Clock = std::chrono::steady_clock;

    std::string label_;
    std::ostream* output_;
    Clock::time_point start_;
};

// 在动态存储期创建 LifecycleProbe，并把唯一所有权交给返回值。
[[nodiscard]] std::unique_ptr<LifecycleProbe> make_owned_probe(
    LifecycleStats& stats,
    int value);

}  // namespace week1_wed_task_a
