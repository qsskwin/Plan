// 引入 task_A 对外公开的全部练习接口。应用只依赖头文件，不直接包含实现文件。
#include "task_a/cpp_basics.hpp"

#include <iostream>  // std::cout、std::boolalpha
#include <string>    // std::string（由字符串计数示例使用）
#include <utility>   // std::move：显式触发所有权转移
#include <vector>    // std::vector<double>

// Week1 / Tue / task_A 的可运行示例入口。
// 这里按照任务清单依次调用各接口，并把关键状态打印出来，便于从运行结果观察语义。
int main() {
    // 练习 1：left 和 right 是 main() 拥有的局部变量，生命周期持续到 main() 结束。
    // 函数接收它们的非常量引用，所以调用后两个原变量的值会真正发生交换。
    int left = 3;
    int right = 7;
    task_a::swap_by_reference(left, right);
    std::cout << "swap: left=" << left << ", right=" << right << '\n';

    // 练习 2：readings 使用 const 修饰，表达示例后续也不打算修改这组数据。
    // sum() 和 mean() 都以 const 引用借用它，因此不会复制 vector，也不能修改元素。
    const std::vector<double> readings{2.0, 4.0, 6.0};
    std::cout << "sum=" << task_a::sum(readings)
              // 已知 readings 非空，所以 optional 一定有值；此处调用 value() 是安全的。
              << ", mean=" << task_a::mean(readings).value() << '\n';

    // 练习 3：新建 tracked 时 copy_count 为 0。
    // 第一次调用按值接收左值，需要构造一个副本，因此函数内看到 1；第二次调用只传递
    // const 引用，不创建新对象，所以看到的仍是原对象记录的 0。
    const task_a::CopyTracked tracked;
    std::cout << "copies (by value)=" << task_a::copies_seen_by_value(tracked)
              << ", copies (by const reference)="
              << task_a::copies_seen_by_const_reference(tracked) << '\n';

    // 练习 4：&pointed_value 取得局部整数的地址。increment_if_not_null() 只借用该地址，
    // 不负责释放对象，但可以通过非 const 指针把原值从 10 修改为 11。
    int pointed_value = 10;
    const bool incremented = task_a::increment_if_not_null(&pointed_value);

    // nullptr 明确表示“没有指向任何对象”。函数应在解引用之前识别并拒绝它。
    const bool null_rejected = !task_a::increment_if_not_null(nullptr);

    // std::boolalpha 让 bool 以 true/false 输出，而不是默认的 1/0。
    std::cout << std::boolalpha << "pointer incremented=" << incremented
              << ", value=" << pointed_value
              << ", null safely rejected=" << null_rejected << '\n';

    // 练习 5：position 内部直接拥有三个 double。scale_vec3() 只读借用 position，
    // 并按值返回一个独立的新数组 doubled，所以 position 自身保持不变。
    const task_a::Vec3 position{1.0, -2.0, 3.0};
    const task_a::Vec3 doubled = task_a::scale_vec3(position, 2.0);
    std::cout << "scaled Vec3=[" << doubled[0] << ", " << doubled[1] << ", "
              << doubled[2] << "]\n";

    // 练习 6：花括号列表先构造一个临时 vector，临时对象会存活到整个函数调用结束。
    // count_strings() 返回的哈希表拥有自己的字符串键，因此临时 vector 随后销毁也没问题。
    const auto counts = task_a::count_strings({"ned", "frd", "ned"});

    // at() 读取已存在键的值；与 operator[] 不同，它不会在读取时意外插入新键。
    std::cout << "word counts: ned=" << counts.at("ned")
              << ", frd=" << counts.at("frd") << '\n';

    // 练习 8：owned 是动态整数 42 的唯一所有者，离开作用域时会自动释放该整数。
    auto owned = task_a::make_owned_int(42);

    // unique_ptr 不能复制，必须用 std::move 显式把所有权交给 consume_owned_int()。
    // 转移后 owned 仍是一个有效的 unique_ptr 对象，但它不再拥有整数，布尔值为 false。
    const auto consumed = task_a::consume_owned_int(std::move(owned));

    // consumed 保存的是独立的 int 值，原动态整数即使已经被释放也不会影响它。
    std::cout << "unique_ptr consumed value=" << consumed.value()
              << ", caller still owns it=" << static_cast<bool>(owned) << '\n';

    // main 返回 0，操作系统以及 CTest 都会把它解释为程序正常结束。
    return 0;
}
