#include "exercise.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <random>
#include <stdexcept>

#include "ldpc_graph_impl.h"
namespace lesson_10 {
srsran::ldpc_graph_impl MakeGraph(const CodingPlan& p) {
  return {p.bg == 1 ? srsran::ldpc_base_graph_type::BG1
                    : srsran::ldpc_base_graph_type::BG2,
          static_cast<srsran::ldpc::lifting_size_t>(p.z)};
}
Bits LdpcEncode(const CodeBlock& block, const CodingPlan& p) {
  // TODO(1): 复制系统位并把filler置0
  // English: Copy systematic bits, zero filler
  // TODO(2): 实现循环移位异或
  // English: Implement shifted XOR
  // TODO(3): 由前四行求p1
  // English: Solve p1 from first four rows
  // TODO(4): 依次求其余校验块
  // English: Solve remaining parity blocks
  // TODO(5): 用H*c=0及独立向量验证
  // English: Check syndrome and independent vectors
  pusch::Fail("NOT IMPLEMENTED: lesson 10");
}
}  // namespace lesson_10
