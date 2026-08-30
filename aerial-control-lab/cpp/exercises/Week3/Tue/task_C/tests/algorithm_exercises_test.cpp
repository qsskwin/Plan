#include "algorithm_exercises.hpp"

#include <gtest/gtest.h>

#include <vector>

TEST(AlgorithmSortTest, SortsValuesAscending) {
  std::vector<double> values{3.0, -1.0, 2.0, 2.0};
  aerial_control::week3_tue::sort_ascending(values);
  const std::vector<double> expected{-1.0, 2.0, 2.0, 3.0};
  EXPECT_EQ(values, expected);
}

TEST(AlgorithmFindTest, ReportsWhetherValueExists) {
  const std::vector<double> values{3.0, -1.0, 2.0, 2.0};
  EXPECT_TRUE(aerial_control::week3_tue::contains_value(values, 2.0));
  EXPECT_FALSE(aerial_control::week3_tue::contains_value(values, 4.0));
  const std::vector<double> empty_values{};
  EXPECT_FALSE(aerial_control::week3_tue::contains_value(empty_values, 2.0));
}

TEST(AlgorithmHasValueAboveTest, ReportsWhetherAnyValueIsAboveThreshold) {
  const std::vector<double> values{3.0, -1.0, 2.0, 2.0};
  EXPECT_TRUE(aerial_control::week3_tue::has_value_above(values, 2.5));
  EXPECT_FALSE(aerial_control::week3_tue::has_value_above(values, 4.0));
  const std::vector<double> empty_values{};
  EXPECT_FALSE(aerial_control::week3_tue::has_value_above(empty_values, 2.0));
  EXPECT_FALSE(aerial_control::week3_tue::has_value_above(values, 3.0));
}

TEST(AlgorithmCountTest, CountsMatchingValues) {
  const std::vector<double> values{2.0, 1.0, 2.0, 3.0, 2.0};
  EXPECT_EQ(aerial_control::week3_tue::count_value(values, 2.0), 3U);
  EXPECT_EQ(aerial_control::week3_tue::count_value(values, 4.0), 0U);
  const std::vector<double> empty_values{};
  EXPECT_EQ(aerial_control::week3_tue::count_value(empty_values, 2.0), 0U);
}

TEST(AlgorithmCountIfTest, CountsValuesStrictlyAboveThreshold) {
  const std::vector<double> values{3.0, -1.0, 2.0, 2.0};
  EXPECT_EQ(aerial_control::week3_tue::count_values_above(values, 2.0), 1U);
  EXPECT_EQ(aerial_control::week3_tue::count_values_above(values, 3.0), 0U);
  const std::vector<double> empty_values{};
  EXPECT_EQ(aerial_control::week3_tue::count_values_above(empty_values, 0.0),
            0U);
  EXPECT_EQ(aerial_control::week3_tue::count_values_above(values, -2.0), 4U);
}

TEST(AlgorithmTransformTest, ReturnsScaledCopyWithoutChangingInput) {
  const std::vector<double> values{1.0, -2.0, 3.5};
  const std::vector<double> expected{2.0, -4.0, 7.0};
  const double scale = 2.0;
  EXPECT_EQ(aerial_control::week3_tue::scaled_copy(values, scale), expected);
  EXPECT_EQ(values, (std::vector<double>{1.0, -2.0, 3.5}));
  const std::vector<double> empty_values{};
  EXPECT_TRUE(
      aerial_control::week3_tue::scaled_copy(empty_values, scale).empty());
}

TEST(AlgorithmEraseRemoveTest, ErasesNegativeValues) {
  std::vector<double> values{-2.0, 0.0, 3.0, -1.0, 4.0};
  aerial_control::week3_tue::erase_negative_values(values);
  const std::vector<double> expected{0.0, 3.0, 4.0};
  EXPECT_EQ(values, expected);
  std::vector<double> empty_values{};
  aerial_control::week3_tue::erase_negative_values(empty_values);
  EXPECT_TRUE(empty_values.empty());
  std::vector<double> all_negative_values{-1.0, -2.0, -3.0};
  aerial_control::week3_tue::erase_negative_values(all_negative_values);
  EXPECT_TRUE(all_negative_values.empty());
}