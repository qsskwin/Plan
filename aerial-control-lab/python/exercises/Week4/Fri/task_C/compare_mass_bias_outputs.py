from pathlib import Path

from exercises.Week4.Fri.task_B.compare_point_mass_outputs import (
    compare_case,
    write_report,
)


REPORT_PATH = Path(__file__).resolve().parent / "mass_bias_comparison_report.md"
MASS_BIAS_SPEC = {
    "slug": "mass_bias",
    "display_name": "Level attitude with 10% mass increase",
    "tf_s": 2.0,
}


def main():
    result = compare_case(MASS_BIAS_SPEC)
    write_report([result], REPORT_PATH)
    prediction_audit = """
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
"""
    with REPORT_PATH.open("a", encoding="utf-8") as report:
        report.write(prediction_audit)
    print(REPORT_PATH.read_text(encoding="utf-8"))
    print(f"Report: {REPORT_PATH}")


if __name__ == "__main__":
    main()
