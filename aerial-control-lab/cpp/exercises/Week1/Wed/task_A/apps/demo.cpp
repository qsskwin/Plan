#include "week1_wed_task_a/lifecycle_raii.hpp"  // 引入生命周期观察器、计时器与工厂函数。

#include <iostream>     // std::cout、std::boolalpha
#include <memory>       // std::unique_ptr
#include <string_view>  // std::string_view：只读借用阶段名称，避免复制字符串。
#include <utility>      // std::move：显式触发移动构造和所有权转移。

// 匿名命名空间让辅助函数只在当前源文件中可见，避免产生外部链接符号。
namespace {

// 打印某个执行阶段累计观察到的生命周期事件。
// stage 和 stats 都以只读方式借用，函数不会取得它们的所有权，也不会修改统计结果。
void print_stats(
    const std::string_view stage,
    const week1_wed_task_a::LifecycleStats& stats) {
    // 一次输出四类计数，便于对照下一条构造、移动或析构语句前后的变化。
    std::cout << stage << ": direct=" << stats.direct_constructions
              << ", copy=" << stats.copy_constructions
              << ", move=" << stats.move_constructions
              << ", destroyed=" << stats.destructions << '\n';
}

}  // namespace：结束仅供本演示文件使用的辅助区域。

// Week1 / Wed / task_A 的可运行演示入口。
// 示例依次展示栈对象生命周期、unique_ptr 唯一所有权和 ScopeTimer 的 RAII 行为。
int main() {
    // using 声明缩短下方类型名称，但不会把整个命名空间的所有符号导入当前作用域。
    using week1_wed_task_a::LifecycleProbe;
    using week1_wed_task_a::LifecycleStats;
    using week1_wed_task_a::ScopeTimer;

    // lifecycle_stats 由 main() 拥有，声明在被观察对象的外层作用域，因此它会比下面三个
    // LifecycleProbe 活得更久，可在对象全部析构后继续读取最终计数。
    LifecycleStats lifecycle_stats;
    {
        // 直接构造 original：保存值 42，并把 direct_constructions 从 0 增加到 1。
        LifecycleProbe original(lifecycle_stats, 42);
        print_stats("after direct construction", lifecycle_stats);

        // copied 是全新的对象：复制 original 的值，并把 copy_constructions 增加到 1。
        LifecycleProbe copied(original);
        print_stats("after copy construction", lifecycle_stats);

        // std::move 只把 original 转换成可绑定到右值引用的表达式，真正的状态处理由
        // LifecycleProbe 的移动构造函数完成。moved 得到 42，original 被设置为 0。
        LifecycleProbe moved(std::move(original));
        print_stats("after move construction", lifecycle_stats);

        // copied 与 moved 都各自拥有整数值 42；它们共享的只是外部统计器引用。
        std::cout << "copied value=" << copied.value()
                  << ", moved value=" << moved.value() << '\n';

        // 离开此花括号时，moved、copied、original 按构造的相反顺序依次析构，
        // 所以 destructions 最终增加 3；移动后的 original 仍然是需要正常析构的对象。
    }
    print_stats("after leaving lifecycle scope", lifecycle_stats);

    // 使用另一份统计器单独记录动态对象，避免与上面的三个栈对象计数混在一起。
    LifecycleStats ownership_stats;

    // make_owned_probe 在动态存储期创建值为 7 的对象。owner 是当前唯一所有者，
    // 可用 operator-> 访问对象；不需要也不应该手工调用 delete。
    auto owner = week1_wed_task_a::make_owned_probe(ownership_stats, 7);
    std::cout << "unique owner value=" << owner->value() << '\n';

    // unique_ptr 禁止拷贝，只能通过 std::move 转移所有权。移动后 owner 仍可安全析构，
    // 但内部指针为空；new_owner 成为动态 LifecycleProbe 的唯一所有者。
    std::unique_ptr<LifecycleProbe> new_owner = std::move(owner);
    std::cout << "ownership transferred=" << std::boolalpha
              << (!owner && static_cast<bool>(new_owner)) << '\n';

    // reset() 立即销毁当前拥有的对象并把 new_owner 置空，因此这里会触发一次析构计数。
    new_owner.reset();
    print_stats("after unique owner reset", ownership_stats);

    // sum 声明在计时作用域之外，保证计时结束后仍能输出累加结果。
    long long sum = 0;
    {
        // timer 构造时立即记录开始时间。把 std::cout 作为非拥有输出流传入时，
        // std::cout 的静态生命周期足以覆盖 timer 的整个局部作用域。
        ScopeTimer timer("accumulate integers", std::cout);

        // 这段循环就是被计时的工作负载。timer 无需显式参与循环。
        for (int value = 0; value < 100000; ++value) {
            sum += value;
        }

        // 离开作用域时自动调用 timer 的析构函数并输出耗时，这就是 RAII 的“自动收尾”。
    }
    std::cout << "sum=" << sum << '\n';

    // 返回 0 表示演示程序正常结束；所有仍存活的自动对象会在返回过程中正常析构。
    return 0;
}
