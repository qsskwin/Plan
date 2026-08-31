#include "controller.hpp"

#include <iostream>
#include <memory>

int main() {
  constexpr double error = -1.5;
  std::unique_ptr<aerial_control::week3_wed::Controller> zero_controller =
      std::make_unique<aerial_control::week3_wed::ZeroController>();

  std::unique_ptr<aerial_control::week3_wed::Controller>
      proportional_controller =
          std::make_unique<
              aerial_control::week3_wed::ProportionalController>(2.0);

  std::cout << "ZeroController output for error " << error << ": "
            << zero_controller->compute(error) << '\n';
  std::cout << "ProportionalController output for error " << error << ": "
            << proportional_controller->compute(error) << '\n';

  return 0;
}
