// 测试通过公开头文件使用练习接口，与真实调用者保持相同的依赖方式。
#include "task_a/cpp_basics.hpp"

#include <cmath>     // std::abs：浮点数近似比较
#include <iostream>  // std::cout、std::cerr：输出测试结果
#include <string>    // std::string：保存失败信息
#include <utility>   // std::move：转移 unique_ptr 所有权
#include <vector>    // std::vector<double>

// Week1 / Tue / task_A 的轻量级 CTest 测试入口。
// 本周尚未正式接入 GoogleTest，所以这里实现最小测试工具，并通过 main() 的退出码
// 告诉 CTest 测试是否通过。后续可以替换测试框架，而不改变被测接口。

// 匿名命名空间使其中的符号只在当前源文件可见，避免与其他测试文件的同名符号冲突。
namespace {

// 累计失败数量，而不是第一次失败就退出，这样一次运行可以报告更多问题。
int failures = 0;

// 通用布尔断言：condition 为 false 时记录错误信息并增加失败计数。
// message 使用 const 引用传递，避免复制字符串；函数只读取它，不修改调用者内容。
void expect(const bool condition, const std::string& message) {
    if (!condition) {
        std::cerr << "FAILED: " << message << '\n';
        ++failures;
    }
}

// 浮点数通常不应直接使用 == 比较，因为二进制浮点运算可能产生微小舍入误差。
// 这里要求实际值与期望值的绝对误差小于 1e-12，再复用 expect() 报告结果。
void expect_near(const double actual, const double expected, const std::string& message) {
    expect(std::abs(actual - expected) < 1e-12, message);
}

}  // namespace：测试辅助符号的内部作用域结束。

int main() {
    // 练习 1：验证引用参数修改的是调用者原变量，而不是函数内部的副本。
    int left = -4;
    int right = 9;
    task_a::swap_by_reference(left, right);
    expect(left == 9 && right == -4, "引用交换应修改调用者的两个整数");

    // 练习 2：同时覆盖普通 vector 和空 vector。
    // value_or(-1.0) 可以在 optional 为空时提供备用值，使测试本身不会抛出异常。
    const std::vector<double> readings{1.0, 2.0, 6.0};
    expect_near(task_a::sum(readings), 9.0, "sum 应计算 vector 元素之和");
    expect_near(task_a::mean(readings).value_or(-1.0), 3.0, "mean 应计算平均值");

    // {} 在这里构造临时空 vector。空集合求和的单位元为 0，而平均值不存在。
    expect_near(task_a::sum({}), 0.0, "空 vector 的和应为零");
    expect(!task_a::mean({}).has_value(), "空 vector 的平均值应为空");

    // 练习 3：tracked 默认构造后复制计数为 0。
    // const 引用调用不产生副本；按值调用传入左值时产生一个副本；原对象始终不被修改。
    const task_a::CopyTracked tracked;
    expect(task_a::copies_seen_by_const_reference(tracked) == 0,
           "const 引用传递不应复制对象");
    expect(task_a::copies_seen_by_value(tracked) == 1, "值传递左值应复制对象");
    expect(tracked.copy_count == 0, "传参不应修改原对象的复制计数");

    // 练习 4：先验证正常指针路径会修改原整数，再验证 nullptr 路径能安全返回 false。
    int pointed_value = 5;
    expect(task_a::increment_if_not_null(&pointed_value), "非空指针应被接受");
    expect(pointed_value == 6, "非空指针指向的值应被修改");
    expect(!task_a::increment_if_not_null(nullptr), "空指针应被安全拒绝");

    // 练习 5：不仅检查返回的新数组数值正确，也检查 const 引用输入没有被修改。
    const task_a::Vec3 vector{1.5, -2.0, 4.0};
    const task_a::Vec3 scaled = task_a::scale_vec3(vector, 2.0);
    expect(scaled == task_a::Vec3{3.0, -4.0, 8.0}, "Vec3 应逐元素缩放");
    expect(vector == task_a::Vec3{1.5, -2.0, 4.0}, "缩放不应修改输入数组");

    // 练习 6：输入包含一个重复字符串。size() 验证不同键的数量，at() 验证各键计数。
    const auto counts = task_a::count_strings({"north", "east", "north", "down"});
    expect(counts.size() == 3, "unordered_map 应包含三个不同字符串");
    expect(counts.at("north") == 2 && counts.at("east") == 1 && counts.at("down") == 1,
           "unordered_map 应正确统计字符串频次");

    // 练习 8：先确认 make_owned_int() 创建了有效所有者以及正确的被管理值。
    auto owned = task_a::make_owned_int(42);
    expect(owned && *owned == 42, "make_owned_int 应返回拥有整数的 unique_ptr");

    // std::move 把唯一所有权交给函数。调用返回后，原 owned 应为空，而 consumed 保存值 42。
    const auto consumed = task_a::consume_owned_int(std::move(owned));
    expect(!owned, "移动后调用者的 unique_ptr 应为空");
    expect(consumed == 42, "接管所有权的函数应能读取对象值");

    // nullptr 可以构造一个空 unique_ptr，用它覆盖没有所有权的边界情况。
    expect(!task_a::consume_owned_int(nullptr).has_value(), "空 unique_ptr 应被安全处理");

    // 只要有任一断言失败，就返回非零退出码，使 CTest 将本测试标记为失败。
    if (failures != 0) {
        std::cerr << failures << " test(s) failed\n";
        return 1;
    }

    // 所有断言通过时打印摘要并返回 0。
    std::cout << "All task A C++ basics tests passed.\n";
    return 0;
}
