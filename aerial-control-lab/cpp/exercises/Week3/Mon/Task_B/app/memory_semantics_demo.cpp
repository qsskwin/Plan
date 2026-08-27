#include <iostream>
#include <string>
#include <memory>
#include <utility>

void modify_by_value(int value)
{
    std::cout << "inside, before modification: "
              << value << '\n';

    // 修改的是局部形参副本。
    value = 99;

    std::cout << "inside, after modification: "
              << value << '\n';
}

void modify_by_reference(int& value)
{
    std::cout << "inside, before modification: "
              << value << '\n';

    // 修改的是调用者对象的引用。
    value = 99;

    std::cout << "inside, after modification: "
              << value << '\n';
}

void inspect_by_const_reference(const int& value)
{
    std::cout << "inside, before modification: "
              << value << '\n';

    // 不能修改引用的值。
    // value = 99; // 编译错误

    std::cout << "inside, after modification: "
              << value << '\n';
}


void modify_if_present(int* value) {
    
    if (value) {
        std::cout << "inside, before modification: "
                  << *value << '\n';

        // 修改的是调用者对象的值。
        *value = 99;

        std::cout << "inside, after modification: "
                  << *value << '\n';
    } else {
        std::cout << "inside, value is null pointer, no modification.\n";
    }
}

std::string make_status_message() {
    std::string status_message{"Status: OK"};
    return status_message;
}
int main()
{
    int value{10};
    int ref_value{10};
    std::cout << "caller, before function: "
              << value << '\n';

    // 值传递不会改变调用者的 value。
    modify_by_value(value);

    std::cout << "caller, after function: "
              << value << '\n';

    // 引用传递会改变调用者的 ref_value。
    std::cout << "caller, before function: "
              << ref_value << '\n';

    modify_by_reference(ref_value);

    std::cout << "caller, after function: "
              << ref_value << '\n';
    
    // 常量引用不会改变调用者的 value。
    std::cout << "caller, before function: "
              << value << '\n';

    inspect_by_const_reference(value);

    std::cout << "caller, after function: "
              << value << '\n';

    int pointer_value{10};

    modify_if_present(&pointer_value); // 传入对象地址
    modify_if_present(nullptr);        // 明确表示没有对象

    std::cout << "caller, after function: "
              << pointer_value << '\n';
              
    std::cout << "Status message: "
              << make_status_message() << '\n';

    auto first = std::make_unique<std::string>("owned resource");
    
    if (first) {
    // first 非空
    std::cout << "first1: " << first->c_str() << std::endl;
    }

    auto second = std::move(first); // 转移所有权

    if (first) {
    // first 非空
    std::cout << "first2: " << first->c_str() << std::endl;
    }

    if (second) {
        // second 非空
        std::cout << "second: " << second->c_str() << std::endl;
    }

    return 0;
}
