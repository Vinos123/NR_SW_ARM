#include "exercise.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <random>
#include <stdexcept>
namespace lesson_07 {

uint32_t CalculateCrc(std::span<const uint8_t> bits, int width,
                      uint32_t polynomial) {
  // TODO(1): 构造低width位掩码
  // English: Build width mask
  // TODO(2): 计算输入与最高位反馈
  // English: Compute feedback
  // TODO(3): 左移并按反馈异或多项式
  // English: Shift and conditionally XOR
  pusch::Fail("NOT IMPLEMENTED: lesson 7");
}
}  // namespace lesson_07
