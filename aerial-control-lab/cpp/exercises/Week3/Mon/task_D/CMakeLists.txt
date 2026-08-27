add_executable(
    week3_mon_task_d_smart_pointers
    app/smart_pointer_demo.cpp
)

target_compile_features(
    week3_mon_task_d_smart_pointers
    PRIVATE cxx_std_17
)

aerial_enable_warnings(
    week3_mon_task_d_smart_pointers
)