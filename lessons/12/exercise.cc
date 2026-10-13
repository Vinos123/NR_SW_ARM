#include "exercise.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <random>
#include <stdexcept>
namespace lesson_12 {
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
std::vector<float> RateRecover(std::span<const float> llrs, const CodingPlan& p,
                               int rv, int qm) {
  // TODO(1): 逆交织
  // English: Undo modulation interleaving
  // TODO(2): 按选择索引累加
  // English: Accumulate selected positions
  // TODO(3): 标记已知filler
  // English: Mark known-zero filler
  // TODO(4): 阅读适配层量化与外部CRC检查
  // English: Inspect quantization and external CRC
  pusch::Fail("NOT IMPLEMENTED: lesson 12");
}
}  // namespace lesson_12
