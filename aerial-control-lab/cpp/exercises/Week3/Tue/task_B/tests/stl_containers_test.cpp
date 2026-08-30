#include "stl_containers.hpp"

#include <deque>
#include <stdexcept>
#include <string>
#include <vector>

#include <gtest/gtest.h>

TEST(ThreeAxisStateTest, MakeThreeAxisState) {
  const auto state =
      aerial_control::week3_tue::make_three_axis_state(1.0, 2.0, 3.0);
  EXPECT_DOUBLE_EQ(state[0], 1.0);
  EXPECT_DOUBLE_EQ(state[1], 2.0);
  EXPECT_DOUBLE_EQ(state[2], 3.0);
  EXPECT_EQ(state.size(), 3U);
}

TEST(VectorMeanTest, ComputesMeanForDynamicallyAddedSamples) {
  std::vector<double> samples;
  samples.push_back(1.0);
  samples.push_back(2.0);
  samples.push_back(3.0);
  samples.push_back(4.0);
  samples.push_back(5.5);

  const double mean_value = aerial_control::week3_tue::mean(samples);
  EXPECT_DOUBLE_EQ(mean_value, 3.1);
}

TEST(VectorMeanTest, RejectsEmptySamples) {
  const std::vector<double> samples{};
  EXPECT_THROW(aerial_control::week3_tue::mean(samples), std::invalid_argument);
}

TEST(RollingBufferTest, KeepsNewestSamplesWithinCapacity) {
  std::deque<double> buffer;
  const std::size_t max_capacity = 3;

  aerial_control::week3_tue::push_rolling_sample(buffer, max_capacity, 1.0);
  aerial_control::week3_tue::push_rolling_sample(buffer, max_capacity, 2.0);
  aerial_control::week3_tue::push_rolling_sample(buffer, max_capacity, 3.0);
  aerial_control::week3_tue::push_rolling_sample(buffer, max_capacity, 4.0);

  EXPECT_EQ(buffer.size(), max_capacity);
  EXPECT_DOUBLE_EQ(buffer[0], 2.0);
  EXPECT_DOUBLE_EQ(buffer[1], 3.0);
  EXPECT_DOUBLE_EQ(buffer[2], 4.0);
}

TEST(RollingBufferTest, LeavesBufferEmptyWhenCapacityIsZero) {
  std::deque<double> buffer;
  const std::size_t max_capacity = 0;

  aerial_control::week3_tue::push_rolling_sample(buffer, max_capacity, 1.0);
  aerial_control::week3_tue::push_rolling_sample(buffer, max_capacity, 2.0);

  EXPECT_TRUE(buffer.empty());
}

TEST(StateCountsTest, CountsRepeatedStates) {
  const std::vector<std::string> states = {"A", "B", "A", "C", "B", "A"};
  const auto counts = aerial_control::week3_tue::count_states(states);

  EXPECT_EQ(counts.at("A"), 3U);
  EXPECT_EQ(counts.at("B"), 2U);
  EXPECT_EQ(counts.at("C"), 1U);
  EXPECT_EQ(counts.size(), 3U);
}

TEST(StateCountsTest, ReturnsEmptyCountsForEmptyStates) {
  const std::vector<std::string> states{};
  const auto counts = aerial_control::week3_tue::count_states(states);
  EXPECT_TRUE(counts.empty());
}

TEST(PriorityQueueTest, ReturnsLargestErrorsInDescendingOrder) {
  const std::vector<double> errors = {1.0, 3.5, 2.2, 4.8, 0.9};
  const std::size_t count = 3;

  const auto largest_errors =
      aerial_control::week3_tue::largest_errors(errors, count);

  EXPECT_EQ(largest_errors.size(), count);
  EXPECT_DOUBLE_EQ(largest_errors[0], 4.8);
  EXPECT_DOUBLE_EQ(largest_errors[1], 3.5);
  EXPECT_DOUBLE_EQ(largest_errors[2], 2.2);
}

TEST(PriorityQueueTest, ReturnsEmptyWhenCountIsZero) {
  const std::vector<double> errors = {1.0, 3.5, 2.2, 4.8, 0.9};
  const std::size_t count = 0;

  const auto largest_errors =
      aerial_control::week3_tue::largest_errors(errors, count);

  EXPECT_TRUE(largest_errors.empty());
}

TEST(PriorityQueueTest, RejectsCountGreaterThanErrorCount) {
  const std::vector<double> errors = {1.0, 3.5, 2.2};
  const std::size_t count = 5;

  EXPECT_THROW(aerial_control::week3_tue::largest_errors(errors, count),
               std::invalid_argument);
}
