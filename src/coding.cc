#include <algorithm>
#include <array>
#include <cmath>
#include <numeric>

#include "ldpc_graph_impl.h"
#include "pusch/core.h"

namespace pusch {
namespace {
constexpr uint32_t kCrc16 = 0x1021;
constexpr uint32_t kCrc24A = 0x864cfb;
constexpr uint32_t kCrc24B = 0x800063;
int DivideUp(int value, int divisor) { return (value + divisor - 1) / divisor; }
srsran::ldpc_graph_impl MakeGraph(const CodingPlan& plan) {
  return {plan.bg == 1 ? srsran::ldpc_base_graph_type::BG1
                       : srsran::ldpc_base_graph_type::BG2,
          static_cast<srsran::ldpc::lifting_size_t>(plan.z)};
}
// 中文：输出索引指向穿孔后的N比特序列；跳过filler但保留重复索引。
// English: Indices address punctured N bits; skip filler, retain repetitions.
// Reference: TS 38.212 V17.6.0, 5.4.2.1. Ncb=N (no LBRM).
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
uint32_t CalculateCrc(std::span<const uint8_t> bits, int width,
                      uint32_t polynomial) {
  Require(width == 16 || width == 24, "CRC width must be 16 or 24");
  uint32_t remainder = 0, mask = (1U << width) - 1;
  // 中文：反馈位是最高寄存器位与输入位的异或，输出为附加CRC。
  // English: Feedback XORs the top register bit with input; result is CRC.
  // Reference: TS 38.212 V17.6.0, 5.1.
  for (uint8_t bit : bits) {
    Require(bit <= 1, "CRC input is not binary");
    bool feedback = ((remainder >> (width - 1)) & 1U) ^ bit;
    remainder = (remainder << 1) & mask;
    if (feedback) remainder ^= polynomial;
  }
  return remainder;
}
Bits AttachCrc(std::span<const uint8_t> bits, int width, uint32_t polynomial) {
  uint32_t crc = CalculateCrc(bits, width, polynomial);
  Bits result(bits.begin(), bits.end());
  for (int i = width - 1; i >= 0; --i) result.push_back((crc >> i) & 1U);
  return result;
}
bool CheckCrc(std::span<const uint8_t> bits, int width, uint32_t polynomial) {
  return CalculateCrc(bits, width, polynomial) == 0;
}
int SelectBaseGraph(int a, float rate) {
  Require(a > 0 && rate > 0 && rate <= 1, "invalid A/R");
  // 中文：三项BG2条件为逻辑或，中间一项包含A与R两个条件。
  // English: OR the three BG2 rules; the middle rule combines A and R.
  // Reference: TS 38.212 V17.6.0, 6.2.2.
  return a <= 292 || (a <= 3824 && rate <= 0.67f) || rate <= 0.25f ? 2 : 1;
}
CodingPlan MakeCodingPlan(int a, float rate) {
  CodingPlan p;
  p.a = a;
  p.bg = SelectBaseGraph(a, rate);
  p.b = a + (a > 3824 ? 24 : 16);
  int kcb = p.bg == 1 ? 8448 : 3840;
  p.c = p.b <= kcb ? 1 : DivideUp(p.b, kcb - 24);
  p.cb_crc = p.c == 1 ? 0 : 24;
  // 中文：主线输入为合法TBS；拒绝不能均分的任意长度，避免隐式补位。
  // English: Require valid equally divisible TB sizes; no implicit padding.
  Require(p.b % p.c == 0, "B must be divisible by C in this profile");
  p.k_prime = p.b / p.c + p.cb_crc;
  p.kb = p.bg == 1 ? 22 : p.b > 640 ? 10 : p.b > 560 ? 9 : p.b > 192 ? 8 : 6;
  p.z = 0;
  for (int z : {2,   3,   4,   5,   6,   7,   8,   9,   10,  11,  12,  13,  14,
                15,  16,  18,  20,  22,  24,  26,  28,  30,  32,  36,  40,  44,
                48,  52,  56,  60,  64,  72,  80,  88,  96,  104, 112, 120, 128,
                144, 160, 176, 192, 208, 224, 240, 256, 288, 320, 352, 384}) {
    if (p.kb * z >= p.k_prime) {
      p.z = z;
      break;
    }
  }
  Require(p.z != 0, "no lifting size found");
  // 中文：BG2即使用较小Kb选择Zc，实际信息区长度仍是10*Zc。
  // English: BG2 uses 10*Zc information bits even if selection Kb is smaller.
  // Reference: TS 38.212 V17.6.0, 5.2.2, Table 5.3.2-1.
  p.k = (p.bg == 1 ? 22 : 10) * p.z;
  p.n = (p.bg == 1 ? 66 : 50) * p.z;
  p.filler = p.k - p.k_prime;
  return p;
}
std::vector<CodeBlock> Segment(std::span<const uint8_t> tb,
                               const CodingPlan& p) {
  Require(static_cast<int>(tb.size()) == p.a, "TB length mismatch");
  auto b = AttachCrc(tb, p.a > 3824 ? 24 : 16, p.a > 3824 ? kCrc24A : kCrc16);
  std::vector<CodeBlock> blocks;
  for (int i = 0; i < p.c; ++i) {
    auto part = std::span<const uint8_t>(b).subspan(i * (p.b / p.c), p.b / p.c);
    Bits data(part.begin(), part.end());
    if (p.c > 1) data = AttachCrc(data, 24, kCrc24B);
    data.resize(p.k, 0);
    std::vector<bool> filler(p.k, false);
    for (int j = p.k_prime; j < p.k; ++j) filler[j] = true;
    blocks.push_back({std::move(data), std::move(filler)});
  }
  return blocks;
}
Bits LdpcEncode(const CodeBlock& block, const CodingPlan& p) {
  Require(static_cast<int>(block.bits.size()) == p.k, "invalid code block");
  auto graph = MakeGraph(p);
  int rows = p.bg == 1 ? 46 : 42, info = p.bg == 1 ? 22 : 10;
  Bits codeword((info + rows) * p.z, 0);
  std::copy(block.bits.begin(), block.bits.end(), codeword.begin());
  for (int i = p.k_prime; i < p.k; ++i) codeword[i] = 0;
  auto accumulate = [&](Bits& sum, int row, int column) {
    auto shift = graph.get_lifted_node(row, column);
    if (shift == srsran::ldpc::NO_EDGE) return;
    for (int j = 0; j < p.z; ++j)
      sum[j] ^= codeword[column * p.z + (j + shift) % p.z];
  };
  // 中文：前四行异或消去后三个校验块，解出第一个校验块。
  // English: XOR the first four rows to eliminate p2..p4 and solve p1.
  // Reference: TS 38.212 V17.6.0, 5.3.2; QC graph structure.
  Bits sum(p.z, 0);
  for (int row = 0; row < 4; ++row)
    for (int col = 0; col < info; ++col) accumulate(sum, row, col);
  int pivot_row = p.bg == 1 ? 1 : 2;
  int shift = graph.get_lifted_node(pivot_row, info);
  for (int j = 0; j < p.z; ++j)
    codeword[p.k + j] = sum[(j + p.z - shift) % p.z];
  // 中文：依次求p2..p4，剩余校验行具有单位对角块。
  // English: Solve p2..p4 in order; remaining rows have identity pivots.
  for (int row = 0; row < rows; ++row) {
    if (row == 3) continue;
    std::fill(sum.begin(), sum.end(), 0);
    int target = row < 3 ? info + row + 1 : info + row;
    int columns = row < 3 ? target : info + 4;
    for (int col = 0; col < columns; ++col) accumulate(sum, row, col);
    std::copy(sum.begin(), sum.end(), codeword.begin() + target * p.z);
  }
  return codeword;
}
bool CheckParity(std::span<const uint8_t> codeword, const CodingPlan& p) {
  Require(static_cast<int>(codeword.size()) == p.n + 2 * p.z,
          "full codeword required for parity check");
  auto graph = MakeGraph(p);
  for (unsigned row = 0; row < graph.get_nof_BG_check_nodes(); ++row) {
    for (int j = 0; j < p.z; ++j) {
      uint8_t parity = 0;
      for (unsigned col = 0; col < graph.get_nof_BG_var_nodes_full(); ++col) {
        auto shift = graph.get_lifted_node(row, col);
        if (shift != srsran::ldpc::NO_EDGE)
          parity ^= codeword[col * p.z + (j + shift) % p.z];
      }
      if (parity) return false;
    }
  }
  return true;
}
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
