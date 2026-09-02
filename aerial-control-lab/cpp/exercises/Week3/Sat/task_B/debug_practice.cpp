#include <iostream>

double applyGain(double error, double gain) {
    return error * gain;
}

double computeCommand(double target, double measured) {
    const double error = target - measured;
    const double gain = 2.0;
    const double command = applyGain(error, gain);
    return command;
}

int main() {
    const double target = 10.0;
    const double measured = 7.0;
    const double command = computeCommand(target, measured);

    std::cout << "expected=6 actual=" << command << '\n';
    return 0;
}
