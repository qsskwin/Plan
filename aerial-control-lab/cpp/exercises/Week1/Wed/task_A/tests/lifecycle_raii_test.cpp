#include "week1_wed_task_a/lifecycle_raii.hpp"  // 被测试的公开接口。

#include <iostream>  // std::cout、std::cerr
#include <memory>    // std::unique_ptr
#include <sstream>   // std::ostringstream：在内存中捕获 ScopeTimer 输出。
#include <string>    // std::string：保存断言消息和计时器输出。
#include <utility>   // std::move

// 测试辅助符号只在本翻译单元中使用，不暴露到最终可执行文件的全局接口。
namespace {

// 记录失败断言数，使测试可以一次执行完多个检查，而不是在第一次失败时立即退出。
int failures = 0;

// 最小断言辅助函数：条件为假时输出可读消息并累计失败次数。
// message 以 const 引用借用，避免复制，同时保证函数不会修改调用者的字符串。
void expect(const bool condition, const std::string& message) {
    if (!condition) {
        std::cerr << "FAILED: " << message << '\n';
        ++failures;
    }
}

}  // namespace

// 测试入口覆盖直接构造、拷贝、移动、析构、唯一所有权以及 RAII 自动计时输出。
int main() {
    // 只引入本测试反复使用的三个类型，让断言主体更紧凑。
    using week1_wed_task_a::LifecycleProbe;
    using week1_wed_task_a::LifecycleStats;
    using week1_wed_task_a::ScopeTimer;

    // 统计器位于内层对象作用域之外，因此三个 LifecycleProbe 析构后仍可检查最终计数。
    LifecycleStats lifecycle_stats;
    {
        // 直接构造应只增加 direct_constructions，且此时对象仍在作用域内、尚未析构。
        LifecycleProbe original(lifecycle_stats, 42);
        expect(lifecycle_stats.direct_constructions == 1,
               "直接构造应被记录一次");

        // 从左值 original 拷贝构造 copied：应记录一次拷贝，并完整保留整数值 42。
        LifecycleProbe copied(original);
        expect(lifecycle_stats.copy_constructions == 1,
               "拷贝构造应被记录一次");
        expect(copied.value() == 42, "拷贝对象应保留原值");

        // std::move 允许调用移动构造函数。目标 moved 接收 42；按照本练习的显式约定，
        // 移动源 original 仍然有效但 value() 变为 0。
        LifecycleProbe moved(std::move(original));
        expect(lifecycle_stats.move_constructions == 1,
               "移动构造应被记录一次");
        expect(moved.value() == 42, "移动目标应接收原值");
        expect(original.value() == 0, "本练习定义的移动源应进入可观察的空状态");

        // 三个对象目前都还在内层作用域中，所以任何析构计数都意味着生命周期实现有误。
        expect(lifecycle_stats.destructions == 0,
               "离开作用域前不应析构三个局部对象");

        // 结束作用域时 moved、copied、original 会按构造的相反顺序自动析构。
    }
    expect(lifecycle_stats.destructions == 3,
           "离开作用域时三个对象都应被析构");

    // 用独立统计器验证动态对象的创建与释放，不受前面局部对象计数影响。
    LifecycleStats ownership_stats;

    // 工厂函数应返回非空 unique_ptr；operator-> 读取的对象值应与构造参数一致。
    auto owner = week1_wed_task_a::make_owned_probe(ownership_stats, 9);
    expect(owner && owner->value() == 9,
           "工厂函数应返回拥有有效对象的 unique_ptr");

    // unique_ptr 的移动应同时满足两个条件：旧指针变空，新指针非空。
    // 这说明所有权发生转移，而不是产生两个能够重复 delete 同一对象的所有者。
    std::unique_ptr<LifecycleProbe> new_owner = std::move(owner);
    expect(!owner && static_cast<bool>(new_owner),
           "移动 unique_ptr 后唯一所有权应转移到新所有者");

    // reset() 主动释放动态对象。LifecycleProbe 的析构函数应随之立刻执行一次。
    new_owner.reset();
    expect(ownership_stats.destructions == 1,
           "unique_ptr 释放所有权时应自动析构对象");

    // ostringstream 由测试拥有，并且比内层 ScopeTimer 活得更久，可安全充当非拥有输出目标。
    std::ostringstream timer_output;
    {
        // timer 构造时开始计时；即使作用域中没有其他语句，离开花括号仍必须自动输出结果。
        ScopeTimer timer("test scope", timer_output);
    }

    // 在计时器析构后取出完整字符串，分别验证固定前缀/标签和微秒单位。
    // 不断言具体耗时数值，因为运行时间会随机器负载和时钟精度变化。
    const std::string timer_message = timer_output.str();
    expect(timer_message.find("[ScopeTimer] test scope:") != std::string::npos,
           "ScopeTimer 析构时应输出标签");
    expect(timer_message.find(" us") != std::string::npos,
           "ScopeTimer 应以微秒输出持续时间");

    // 任意断言失败时返回非零退出码，让 CTest 将该测试标记为失败。
    if (failures != 0) {
        std::cerr << failures << " test(s) failed\n";
        return 1;
    }

    // 所有检查通过时输出摘要并返回 0。
    std::cout << "All Week1 Wednesday task A tests passed.\n";
    return 0;
}
