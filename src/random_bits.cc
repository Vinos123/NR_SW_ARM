#include <random>

#include "pusch/core.h"

namespace pusch {
Bits MakeBits(int count, uint32_t seed) {
  Require(count >= 0, "negative bit count");
  std::mt19937 generator(seed);
  Bits bits(count);
  for (auto& bit : bits) bit = generator() & 1U;
  return bits;
}
}  // namespace pusch
