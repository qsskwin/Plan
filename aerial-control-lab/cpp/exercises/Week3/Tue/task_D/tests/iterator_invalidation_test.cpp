#include <gtest/gtest.h>

#include <cstdint>
#include <iterator>
#include <list>
#include <string>
#include <unordered_map>
#include <vector>

TEST(VectorInvalidationTest, ReallocationChangesStorage) {
  std::vector<int> values;
  values.reserve(2);
  values.resize(values.capacity(), 10);
  const auto old_capacity = values.capacity();
  const auto old_address = reinterpret_cast<std::uintptr_t>(values.data());
  values.push_back(20);
  const auto new_address = reinterpret_cast<std::uintptr_t>(values.data());
  EXPECT_NE(old_address, new_address);
  EXPECT_GT(values.capacity(), old_capacity);
}

TEST(VectorInvalidationTest, EraseKeepsOnlyEarlierIteratorsValid) {
  std::vector<int> values{10, 20, 30, 40};
  const auto first = values.begin();  // 指向 10
  const auto old_capacity = values.capacity();
  const auto next = values.erase(values.begin() +
                                 1);  // 删除 20 删除位置及其后的旧迭代器均失效
  EXPECT_EQ(*first, 10);
  EXPECT_EQ(*next, 30);
  EXPECT_EQ(next, values.begin() + 1);
  EXPECT_EQ(values.capacity(), old_capacity);
  EXPECT_EQ(values.size(), 3U);
  EXPECT_EQ(values, (std::vector<int>{10, 30, 40}));
}

TEST(ListInvalidationTest, InsertKeepsExistingIteratorsValid) {
  std::list<int> values{10, 30};
  const auto first = values.begin();              // 指向 10
  const auto next = std::next(values.begin());    // 指向 30
  const auto inserted = values.insert(next, 20);  // 在 10 和 30 之间插入 20
  EXPECT_EQ(*first, 10);
  EXPECT_EQ(*next, 30);
  EXPECT_EQ(*inserted, 20);
  EXPECT_EQ(values, (std::list<int>{10, 20, 30}));
}

TEST(UnorderedMapInvalidationTest,
     RehashInvalidatesIteratorsButKeepsElementPointersValid) {
  std::unordered_map<std::string, int> counts{{"armed", 1}};

  const auto it = counts.find("armed");
  int* const value_pointer = &it->second;

  const auto old_bucket_count = counts.bucket_count();
  counts.rehash(old_bucket_count * 2 + 1);
  EXPECT_EQ(*value_pointer, 1);  // 元素指针仍然有效
  EXPECT_EQ(value_pointer, &counts.at("armed"));
  EXPECT_LT(old_bucket_count, counts.bucket_count());
}

TEST(VectorEraseTraversalTest, UsesEraseReturnValueToContinueSafely) {
  std::vector<int> values{1, -2, -3, 4, -5};
  auto it = values.begin();

  while (it != values.end()) {
    if (*it < 0) {  // 当前元素需要删除
      it = values.erase(it);
    } else {
      ++it;
    }
  }

  const std::vector<int> expected{1, 4};
  EXPECT_EQ(values, expected);
}