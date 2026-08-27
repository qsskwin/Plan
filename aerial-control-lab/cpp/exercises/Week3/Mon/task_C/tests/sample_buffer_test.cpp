#include "sample_buffer.hpp"

#include <stdexcept>
#include <utility>

#include <gtest/gtest.h>

using aerial_control::week3_mon::SampleBuffer;

TEST(SampleBufferTest, DefaultConstructedBufferIsEmpty) {
  const SampleBuffer buffer;

  EXPECT_TRUE(buffer.empty());
  EXPECT_EQ(buffer.size(), 0U);
}

TEST(SampleBufferTest, AddUpdatesEmptyAndSize) {
  SampleBuffer buffer;

  buffer.add(1.25);
  EXPECT_FALSE(buffer.empty());
  EXPECT_EQ(buffer.size(), 1U);

  buffer.add(-2.5);
  EXPECT_EQ(buffer.size(), 2U);
}

TEST(SampleBufferTest, AtReturnsSamplesInInsertionOrder) {
  SampleBuffer buffer;
  buffer.add(1.25);
  buffer.add(-2.5);
  buffer.add(3.75);

  EXPECT_DOUBLE_EQ(buffer.at(0), 1.25);
  EXPECT_DOUBLE_EQ(buffer.at(1), -2.5);
  EXPECT_DOUBLE_EQ(buffer.at(2), 3.75);
}

TEST(SampleBufferTest, AtThrowsForOutOfRangeIndex) {
  SampleBuffer buffer;

  EXPECT_THROW(
      static_cast<void>(buffer.at(0)),
      std::out_of_range);

  buffer.add(4.0);
  EXPECT_THROW(
      static_cast<void>(buffer.at(1)),
      std::out_of_range);
}

TEST(SampleBufferTest, CopyCreatesIndependentBuffer) {
  SampleBuffer original;
  original.add(1.0);

  SampleBuffer copy = original;
  copy.add(2.0);

  EXPECT_EQ(original.size(), 1U);
  EXPECT_DOUBLE_EQ(original.at(0), 1.0);
  EXPECT_EQ(copy.size(), 2U);
  EXPECT_DOUBLE_EQ(copy.at(0), 1.0);
  EXPECT_DOUBLE_EQ(copy.at(1), 2.0);
}

TEST(SampleBufferTest, MoveTransfersSamplesToDestination) {
  SampleBuffer source;
  source.add(1.0);
  source.add(2.0);

  SampleBuffer destination = std::move(source);

  EXPECT_EQ(destination.size(), 2U);
  EXPECT_DOUBLE_EQ(destination.at(0), 1.0);
  EXPECT_DOUBLE_EQ(destination.at(1), 2.0);

  // 不假设移动后 source 的内容；重新赋值以建立明确状态。
  source = SampleBuffer{};
  EXPECT_TRUE(source.empty());
  source.add(3.0);
  EXPECT_DOUBLE_EQ(source.at(0), 3.0);
}
