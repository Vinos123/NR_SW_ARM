#include "exercise.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <random>
#include <stdexcept>
namespace lesson_16 {

std::vector<Complex> ApplyChannel(std::span<const Complex> input,
                                  std::span<const Complex> taps) {
  // TODO(1): 初始化零输出
  // English: Initialize zero output
  // TODO(2): 遍历有效延迟抽头
  // English: Visit causal channel taps
  // TODO(3): 累加h[j]*x[i-j]
  // English: Accumulate convolution
  pusch::Fail("NOT IMPLEMENTED: lesson 16");
}
}  // namespace lesson_16
