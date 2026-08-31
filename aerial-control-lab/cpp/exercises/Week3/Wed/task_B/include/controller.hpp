#pragma once

namespace aerial_control::week3_wed {

class Controller {
 public:
  // error is altitude error in m; the return value is thrust correction in N.
  virtual double compute(double error) const = 0;

  virtual ~Controller() = default;
};

class ZeroController : public Controller {
 public:
  double compute(double error) const override;
};

class ProportionalController : public Controller {
 public:
  // gain is proportional gain in N/m.
  explicit ProportionalController(double gain);

  double compute(double error) const override;

 private:
  double gain_;
};

}  // namespace aerial_control::week3_wed
