# Week 4 point-mass cross-language comparison

Every saved time and every state component was compared with `rtol=0`. Position uses `atol=1e-10 m`; velocity uses `atol=1e-10 m/s`. The analytical trajectories below are constructed independently by this comparison program.

| Case | Samples | Python to analytical position (m) | Python to analytical velocity (m/s) | C++ to analytical position (m) | C++ to analytical velocity (m/s) | Python to C++ position (m) | Python to C++ velocity (m/s) |
|---|---:|---:|---:|---:|---:|---:|---:|
| Level attitude with 10% mass increase | 201 | 2.886579864025407e-15 | 4.6629367034256575e-15 | 2.886579864025407e-15 | 4.6629367034256575e-15 | 0 | 0 |

All six comparisons for every case satisfy their absolute tolerance.

## Prediction audit and physical conclusion

The learner predicted positive Down motion and wrote
`a_z = 1.65*g - 14.709975`, followed by `p_z=0.5*a_z*t^2` and
`v_z=a_z*t`. The direction and constant-acceleration kinematics were correct,
but `1.65*g - 14.709975 = 1.4709975 N` is net force, not acceleration.

The corrected NED Down acceleration is

`a_d = (1.65*g - 14.709975)/1.65 = 0.8915136363636364 m/s^2`.

At `t=2 s`, the analytical position is
`[0,0,1.783027272727273] m` and velocity is
`[0,0,1.783027272727273] m/s`. Python and C++ produce identical saved
trajectories for this case.

The original hover thrust is a fixed feedforward input sized for `1.5 kg`.
After the mass changes to `1.65 kg`, thrust is smaller than weight, so the
vehicle accelerates Down. Because the input does not use position or velocity,
it cannot detect the deviation or restore equilibrium.

For Week 5, feedback must use vertical position and velocity. If altitude is
defined as `h=-p_d`, then `h_dot=-v_d`; alternatively, a controller may use
`p_d` and `v_d` directly. The reference, error sign, and thrust sign must use
one convention consistently before implementing the controller.
