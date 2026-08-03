# 悬空引用错误示例（不要编译或运行）

> 所属练习：`Week1/Tue/task_A`

下面的函数返回局部变量的引用。`local_value` 由函数栈帧拥有，只活到函数返回；返回后引用仍指向已经结束生命周期的对象，读取它属于未定义行为。

```cpp
const int& bad_reference() {
    const int local_value = 42;
    return local_value;  // 错误：函数返回时 local_value 的生命周期结束。
}
```

调用者并不拥有该局部对象，也不能通过保存引用延长它的生命周期。正确做法是按值返回：

```cpp
int safe_value() {
    const int local_value = 42;
    return local_value;
}
```

按值返回的结果由调用者接收并拥有，返回值的生命周期与函数内的局部变量无关。此错误示例只保存在文档中，没有进入正式库或任何构建目标。
