#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <random>
#include <stdexcept>

#include "exercise.h"
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
  pusch::Require((qm == 2 || qm == 4) && e % qm == 0 &&
                     static_cast<int>(full.size()) == p.n + 2 * p.z,
                 "invalid rate input");
  auto indices = Selection(p, e, rv);
  Bits output(e);
  // 中文：先选比特，再按调制阶数交织；前2Zc比特永不发送。
  // English: Select then interleave by Qm; the first 2Zc bits are punctured.
  // Reference: TS 38.212 V17.6.0, 5.4.2.2.
  for (int j = 0; j < e / qm; ++j)
    for (int i = 0; i < qm; ++i)
      output[j * qm + i] = full[2 * p.z + indices[i * (e / qm) + j]];
  return output;
}
}  // namespace lesson_11
