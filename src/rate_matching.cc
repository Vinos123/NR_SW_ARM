#include <algorithm>

#include "pusch/core.h"

namespace pusch {
namespace {
// 中文：选择穿孔后N比特坐标，跳过filler、保留重复。
// English: Select punctured N-bit positions; skip filler, keep repeats.
// Reference: TS 38.212 V17.6.0, 5.4.2.1; Ncb=N.
std::vector<int> Selection(const CodingPlan& p, int e, int rv) {
  Require(e > 0 && rv >= 0 && rv < 4, "invalid rate matching request");
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
}  // namespace

Bits RateMatch(std::span<const uint8_t> full, const CodingPlan& p, int e,
               int rv, int qm) {
  Require((qm == 2 || qm == 4) && e % qm == 0 &&
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

std::vector<float> RateRecover(std::span<const float> llrs, const CodingPlan& p,
                               int rv, int qm) {
  int e = static_cast<int>(llrs.size());
  Require((qm == 2 || qm == 4) && e % qm == 0, "invalid LLR count");
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

std::vector<int> BlockLengths(int g, int blocks, int qm) {
  Require(g > 0 && blocks > 0 && (qm == 2 || qm == 4) && g % qm == 0,
          "invalid G/C/Qm");
  int symbols = g / qm, remainder = symbols % blocks;
  std::vector<int> result(blocks);
  for (int r = 0; r < blocks; ++r)
    result[r] = qm * (symbols / blocks + (r >= blocks - remainder));
  return result;
}
}  // namespace pusch
