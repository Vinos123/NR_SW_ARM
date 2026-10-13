#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <random>
#include <stdexcept>

#include "exercise.h"
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
  int e = static_cast<int>(llrs.size());
  pusch::Require((qm == 2 || qm == 4) && e % qm == 0, "invalid LLR count");
  auto indices = Selection(p, e, rv);
  std::vector<float> output(p.n, 0);
  // 中文：重复发送的LLR相加，未发送位为0；filler是已知的0。
  // English: Sum repeated LLRs; punctured bits are 0; filler is known zero.
  for (int j = 0; j < e / qm; ++j)
    for (int i = 0; i < qm; ++i)
      output[indices[i * (e / qm) + j]] += llrs[j * qm + i];
  for (int i = std::max(0, p.k_prime - 2 * p.z); i < p.k - 2 * p.z; ++i)
    output[i] = 10000.f;
  return output;
}
}  // namespace lesson_12
