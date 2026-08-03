#include "core/project_info.hpp"

namespace aerial_control {

std::string_view project_name() noexcept {
    return "aerial-control-lab";
}

}  // namespace aerial_control
// no exception = noexcpet 承诺函数不会出现C++异常。
// 1. 给编译器优化提示
// 编译器可以跳过异常栈展开相关代码，生成更精简机器码；
// 2. 类型系统约束
// 某些标准容器（std::vector）移动构造、函数包装器会区分 noexcept；
// 3. 代码可读性：文档作用
// 告诉开发者：调用此函数不必写 try-catch；
// 4. 强制约束（C++17 之后）
// 如果函数内部抛出异常，程序会直接调用 std::terminate() 崩溃，不会向上传播异常。
