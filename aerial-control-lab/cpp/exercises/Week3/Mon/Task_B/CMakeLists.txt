add_executable(
    week3_mon_task_b_memory_semantics
    app/memory_semantics_demo.cpp
)

target_compile_features(
    week3_mon_task_b_memory_semantics
    PRIVATE cxx_std_17
)

aerial_enable_warnings(
    week3_mon_task_b_memory_semantics
)