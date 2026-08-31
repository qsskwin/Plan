#include "controller.hpp"

#include <gtest/gtest.h>

#include <limits>
#include <memory>
#include <stdexcept>

namespace {

using aerial_control::week3_wed::Controller;
using aerial_control::week3_wed::ProportionalController;
using aerial_control::week3_wed::ZeroController;

class ControllerTest : public ::testing::Test {
 protected:
  void SetUp() override {
    zero_controller_ = std::make_unique<ZeroController>();
    proportional_controller_ =
        std::make_unique<ProportionalController>(gain_);
  }

  double gain_{2.0};
  double zero_error_{0.0};
  double positive_error_{1.5};
  double negative_error_{-1.5};
  double expected_positive_output_{3.0};
  double expected_negative_output_{-3.0};
  std::unique_ptr<Controller> zero_controller_;
  std::unique_ptr<Controller> proportional_controller_;
};

TEST_F(ControllerTest, ZeroControllerReturnsZeroForZeroError) {
  EXPECT_DOUBLE_EQ(zero_controller_->compute(zero_error_), 0.0);
}

TEST_F(ControllerTest, ProportionalControllerReturnsZeroForZeroError) {
  EXPECT_DOUBLE_EQ(proportional_controller_->compute(zero_error_), 0.0);
}

TEST_F(ControllerTest, ProportionalControllerScalesPositiveError) {
  EXPECT_DOUBLE_EQ(proportional_controller_->compute(positive_error_),
                   expected_positive_output_);
}

TEST_F(ControllerTest, ProportionalControllerScalesNegativeError) {
  EXPECT_DOUBLE_EQ(proportional_controller_->compute(negative_error_),
                   expected_negative_output_);
}

TEST_F(ControllerTest, ProportionalControllerRejectsNonFiniteGain) {
  EXPECT_THROW(
      ProportionalController{std::numeric_limits<double>::infinity()},
      std::invalid_argument);
  EXPECT_THROW(ProportionalController{
                   std::numeric_limits<double>::quiet_NaN()},
               std::invalid_argument);
}

TEST_F(ControllerTest, ZeroControllerRejectsNonFiniteError) {
  EXPECT_THROW(
      zero_controller_->compute(std::numeric_limits<double>::quiet_NaN()),
      std::invalid_argument);
}

TEST_F(ControllerTest, ProportionalControllerRejectsNonFiniteError) {
  EXPECT_THROW(proportional_controller_->compute(
                   std::numeric_limits<double>::infinity()),
               std::invalid_argument);
}

TEST_F(ControllerTest, CallsDerivedImplementationThroughBaseInterface) {
  std::unique_ptr<Controller> controller =
      std::make_unique<ProportionalController>(gain_);

  EXPECT_DOUBLE_EQ(controller->compute(positive_error_),
                   expected_positive_output_);
}

}  // namespac