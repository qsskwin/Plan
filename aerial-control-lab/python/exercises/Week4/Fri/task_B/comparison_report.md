# Week 4 point-mass cross-language comparison

Every saved time and every state component was compared with `rtol=0`. Position uses `atol=1e-10 m`; velocity uses `atol=1e-10 m/s`. The analytical trajectories below are constructed independently by this comparison program.

| Case | Samples | Python to analytical position (m) | Python to analytical velocity (m/s) | C++ to analytical position (m) | C++ to analytical velocity (m/s) | Python to C++ position (m) | Python to C++ velocity (m/s) |
|---|---:|---:|---:|---:|---:|---:|---:|
| Free fall | 201 | 2.1316282072803006e-14 | 3.907985046680551e-14 | 2.1316282072803006e-14 | 3.907985046680551e-14 | 0 | 0 |
| Level hover | 1001 | 0 | 0 | 0 | 0 | 0 | 0 |
| Fixed positive roll | 201 | 9.3258734068513149e-15 | 1.3322676295501878e-14 | 9.3258734068513149e-15 | 1.3322676295501878e-14 | 0 | 0 |

All six comparisons for every case satisfy their absolute tolerance.
