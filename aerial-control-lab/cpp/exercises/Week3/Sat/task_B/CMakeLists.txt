add_executable(
    week3_sat_task_b_gdb_practice
    debug_practice.cpp
)

target_compile_features(
    week3_sat_task_b_gdb_practice
    PRIVATE cxx_std_17
)

aerial_enable_warnings(
    week3_sat_task_b_gdb_practice
)
