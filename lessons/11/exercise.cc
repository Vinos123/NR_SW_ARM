#include "exercise.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <random>
#include <stdexcept>
namespace lesson_11 {
std::vector<int> Selection(const CodingPlan& p, int e, int rv) {
  pusch::Require(e > 0 && rv >= 0 && rv < 4, "invalid rate matching request");
  constexpr int kStart[2][4] = {{0, 17, 33, 56}, {0, 13, 25, 43}};
  int position = kStart[p.bg - 1][rv] * p.z;
  std::vector<int> selected;
  while (static_cast<int>(selected.size()) < e) {
    int full_index = position + 2 * p.z;
    if (!(full_index >= p.k_prime && full_index < p.k))
      selected.push_back(position);
    position = (position + 1) % p.n;
  }
  return selected;
}
Bits RateMatch(std::span<const uint8_t> full, const CodingPlan& p, int e,
               int rv, int qm) {
  // TODO(1): 阅读并实现Selection辅助函数
  // English: Implement circular selection
  // TODO(2): 加回2Zc定位完整码字
  // English: Offset into full codeword
  // TODO(3): 按Qm写出交织索引
  // English: Interleave by Qm
  pusch::Fail("NOT IMPLEMENTED: lesson 11");
}
}  // namespace lesson_11
