1.TEST、EXPECT_*、ASSERT_* 各自做什么？
TEST就是定义组和测试的名字，然后使用googletest提供的api进行测试。EXPECT_*：失败后继续执行当前测试；ASSERT_*：失败后立即结束当前测试函数，但不会停止其他测试。这里的 * 是通配表示，实际使用的是 EXPECT_TRUE、EXPECT_DOUBLE_EQ、ASSERT_EQ 等具体宏。

2.测试可执行文件由谁编译？
测试程序由 CMake 描述和组织构建，由生成器对应的构建工具调度，最终由 C++ 编译器和链接器生成；GoogleTest 和 CTest 都不是编译器。

3.CTest 如何知道有哪些测试?
CTest 通过 CMake 配置获得测试注册信息。普通测试可以用 add_test 直接注册；GoogleTest 可以用 gtest_discover_tests 执行测试程序的 --gtest_list_tests，发现其中的 TEST(...)，再分别注册给 CTest。GTest::gtest_main 只负责链接，include(GoogleTest) 只负责加载辅助命令。

TEST(ClampValueTest, ReturnsValueWithinBounds)
        ↓ 编译
进入 GoogleTest 程序内部的测试注册表
        ↓
gtest_discover_tests 在构建后运行：
test.exe --gtest_list_tests
        ↓
获取所有 GoogleTest 测试名称
        ↓
分别注册为 CTest 测试
        ↓
CTest 运行某一项时调用：
test.exe --gtest_filter=ClampValueTest.ReturnsValueWithinBounds

4.为什么“编译测试程序”和“运行测试”不是一回事？
编译测试程序是把测试源代码和相关库编译、链接成可执行文件。这个阶段主要检查语法、类型以及链接是否正确，但不会执行 TEST 中的断言。
运行测试是启动已经生成的测试可执行文件，由 GoogleTest 执行测试用例并判断断言是否通过，CTest 负责启动测试并汇总结果

5.如何只运行某一个测试用例？
可以在 VS Code 的测试面板中点击单个测试用例的运行按钮。CMake Tools 会调用 CTest，并根据测试名称筛选，只运行所选测试。也可以使用 ctest -R 在命令行实现相同效果。