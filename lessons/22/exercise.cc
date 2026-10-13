#include "exercise.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <random>
#include <stdexcept>
namespace lesson_22 {

void QpskLlrScalar(std::span<const float> iq, float variance,
                   std::span<float> output) {
  // TODO(1): 检查输入输出长度
  // English: Validate spans
  // TODO(2): 计算2sqrt(2)/N0
  // English: Compute scale
  // TODO(3): 逐元素乘法
  // English: Scale every float
  // TODO(4): 用CLI bench记录基线
  // English: Record CLI benchmark
  pusch::Fail("NOT IMPLEMENTED: lesson 22");
}
}  // namespace lesson_22
