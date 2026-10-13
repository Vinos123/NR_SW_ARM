#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <random>
#include <stdexcept>

#include "exercise.h"
namespace lesson_03 {

Bits MakeBits(int count, uint32_t seed) {
  pusch::Require(count >= 0, "negative bit count");
  std::mt19937 generator(seed);
  Bits bits(count);
  for (auto& bit : bits) bit = generator() & 1U;
  return bits;
}
}  // namespace lesson_03
