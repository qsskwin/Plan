#include "week1_wed_task_a/lifecycle_raii.hpp"

#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <utility>

namespace {

int failures = 0;

void expect(const bool condition, const std::string& message) {
    if (!condition) {
        std::cerr << "FAILED: " << message << '\n';
        ++failures;
    }
}

}  // namespace

int main() {
    using week1_wed_task_a::LifecycleProbe;
    using week1_wed_task_a::LifecycleStats;
    using week1_wed_task_a::ScopeTimer;

    LifecycleStats lifecycle_stats;
    {
        LifecycleProbe original(lifecycle_stats, 42);
        expect(lifecycle_stats.direct_constructions == 1,
               "直接构造应被记录一次");

        LifecycleProbe copied(original);
        expect(lifecycle_stats.copy_constructions == 1,
               "拷贝构造应被记录一次");
        expect(copied.value() == 42, "拷贝对象应保留原值");

        LifecycleProbe moved(std::move(original));
        expect(lifecycle_stats.move_constructions == 1,
               "移动构造应被记录一次");
        expect(moved.value() == 42, "移动目标应接收原值");
        expect(original.value() == 0, "本练习定义的移动源应进入可观察的空状态");
        expect(lifecycle_stats.destructions == 0,
               "离开作用域前不应析构三个局部对象");
    }
    expect(lifecycle_stats.destructions == 3,
           "离开作用域时三个对象都应被析构");

    LifecycleStats ownership_stats;
    auto owner = week1_wed_task_a::make_owned_probe(ownership_stats, 9);
    expect(owner && owner->value() == 9,
           "工厂函数应返回拥有有效对象的 unique_ptr");

    std::unique_ptr<LifecycleProbe> new_owner = std::move(owner);
    expect(!owner && static_cast<bool>(new_owner),
           "移动 unique_ptr 后唯一所有权应转移到新所有者");
    new_owner.reset();
    expect(ownership_stats.destructions == 1,
           "unique_ptr 释放所有权时应自动析构对象");

    std::ostringstream timer_output;
    {
        ScopeTimer timer("test scope", timer_output);
    }
    const std::string timer_message = timer_output.str();
    expect(timer_message.find("[ScopeTimer] test scope:") != std::string::npos,
           "ScopeTimer 析构时应输出标签");
    expect(timer_message.find(" us") != std::string::npos,
           "ScopeTimer 应以微秒输出持续时间");

    if (failures != 0) {
        std::cerr << failures << " test(s) failed\n";
        return 1;
    }

    std::cout << "All Week1 Wednesday task A tests passed.\n";
    return 0;
}
