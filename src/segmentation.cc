#include <utility>

#include "pusch/core.h"

namespace pusch {
namespace {
constexpr uint32_t kCrc16 = 0x1021;
constexpr uint32_t kCrc24A = 0x864cfb;
constexpr uint32_t kCrc24B = 0x800063;
}  // namespace

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
}  // namespace pusch
