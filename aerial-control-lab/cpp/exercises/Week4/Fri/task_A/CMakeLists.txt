add_executable(point_mass_demo
    app/point_mass_demo.cpp
)

target_link_libraries(point_mass_demo PRIVATE aerial_core)
target_compile_features(point_mass_demo PRIVATE cxx_std_17)
aerial_enable_warnings(point_mass_demo)
