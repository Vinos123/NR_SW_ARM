#include "exercise.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <random>
#include <stdexcept>
namespace lesson_05 {

std::vector<Complex> Modulate(std::span<const uint8_t> bits, int qm) {
  // TODO(1): 验证Qm和整符号长度
  // English: Validate Qm and length
  // TODO(2): 把b0/b1映射到符号正负
  // English: Map b0/b1 to signs
  // TODO(3): 用b2/b3确定16QAM幅度
  // English: Set 16QAM amplitudes
  // TODO(4): 除以归一化因子
  // English: Normalize energy
  pusch::Fail("NOT IMPLEMENTED: lesson 5");
}
}  // namespace lesson_05
