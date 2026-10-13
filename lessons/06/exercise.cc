#include "exercise.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <random>
#include <stdexcept>
namespace lesson_06 {

std::vector<float> Demodulate(std::span<const Complex> symbols, int qm,
                              float noise_variance) {
  // TODO(1): 枚举星座及标签
  // English: Enumerate constellation and labels
  // TODO(2): 分别寻找bit0/bit1最短距离
  // English: Find both minimum distances
  // TODO(3): 计算(d1-d0)/N0
  // English: Form the positive-for-zero LLR
  pusch::Fail("NOT IMPLEMENTED: lesson 6");
}
}  // namespace lesson_06
