#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <random>
#include <stdexcept>

#include "exercise.h"
#include "ldpc_graph_impl.h"
namespace lesson_10 {
srsran::ldpc_graph_impl MakeGraph(const CodingPlan& p) {
  return {p.bg == 1 ? srsran::ldpc_base_graph_type::BG1
                    : srsran::ldpc_base_graph_type::BG2,
          static_cast<srsran::ldpc::lifting_size_t>(p.z)};
}
Bits LdpcEncode(const CodeBlock& block, const CodingPlan& p) {
  pusch::Require(static_cast<int>(block.bits.size()) == p.k,
                 "invalid code block");
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
}  // namespace lesson_10
