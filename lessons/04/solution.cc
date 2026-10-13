#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <random>
#include <stdexcept>

#include "exercise.h"
namespace lesson_04 {

Bits Gold(uint32_t c_init, int count) {
  pusch::Require(count >= 0 && c_init < (1U << 31), "invalid Gold parameters");
  // 中文：先推进Nc=1600，再输出x1(n+Nc) XOR x2(n+Nc)。
  // English: Skip Nc=1600; output x1(n+Nc) XOR x2(n+Nc).
  // Reference: TS 38.211 V17.5.0, 5.2.1.
  constexpr int kNc = 1600;
  Bits x1(count + kNc + 31, 0), x2(x1.size(), 0), result(count);
  x1[0] = 1;
  for (int i = 0; i < 31; ++i) x2[i] = (c_init >> i) & 1U;
  for (int i = 0; i < count + kNc; ++i) {
    x1[i + 31] = x1[i + 3] ^ x1[i];
    x2[i + 31] = x2[i + 3] ^ x2[i + 2] ^ x2[i + 1] ^ x2[i];
  }
  for (int i = 0; i < count; ++i) result[i] = x1[i + kNc] ^ x2[i + kNc];
  return result;
}
}  // namespace lesson_04
