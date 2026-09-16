add_executable(week5_mon_task_c_rk4_convergence
    rk4_convergence.cpp
)

target_compile_features(
    week5_mon_task_c_rk4_convergence
    PRIVATE cxx_std_17
)

target_link_libraries(
    week5_mon_task_c_rk4_convergence
    PRIVATE aerial_core
)

aerial_enable_warnings(
    week5_mon_task_c_rk4_convergence
)