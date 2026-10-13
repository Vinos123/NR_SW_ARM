// 中文：首先包含被测头文件，避免GoogleTest间接提供缺失的标准库依赖。
// English: Include the tested header first so GoogleTest cannot mask
// dependencies.
#include <utility>
#include <vector>

#include "gtest/gtest.h"
#include "srsran/adt/bounded_bitset.h"

TEST(PortableBitset, CallbacksWithSelfContainedHeader) {
  srsran::bounded_bitset<8> bits(8);
  bits.set(1);
  bits.set(2);
  bits.set(7);

  // 中文：覆盖两个使用std::function检查回调签名的模板接口。
  // English: Exercise both templates that check callbacks with std::function.
  std::vector<size_t> positions;
  bits.for_each(0, bits.size(), [&positions](size_t position) {
    positions.push_back(position);
  });
  EXPECT_EQ(positions, (std::vector<size_t>{1, 2, 7}));

  // 中文：区间采用左闭右开索引，最后一段终止于bitset长度。
  // English: Intervals are half-open; the last interval ends at the bitset
  // size.
  std::vector<std::pair<size_t, size_t>> intervals;
  srsran::for_each_interval(bits, [&intervals](size_t begin, size_t end) {
    intervals.emplace_back(begin, end);
  });
  EXPECT_EQ(intervals,
            (std::vector<std::pair<size_t, size_t>>{{1, 3}, {7, 8}}));
}
