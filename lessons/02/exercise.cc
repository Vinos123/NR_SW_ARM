#include "exercise.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <random>
#include <stdexcept>
namespace lesson_02 {

int GridIndex(int symbol, int subcarrier, int tones) {
  // TODO(1): 检查l和k边界
  // English: Check symbol and subcarrier bounds
  if (symbol < 0 || symbol >= 14 || subcarrier <= 0 || subcarrier > tones)
    throw std::invalid_argument("symbol or subcarrier invalid.");
  // TODO(2): 计算连续内存偏移
  // English: Compute contiguous storage offset
  int offset = symbol * tones + subcarrier;
  return offset;
  // TODO(3): 在ResourceGrid中查看引用返回
  // English: Inspect reference-returning accessors
  pusch::Fail("NOT IMPLEMENTED: lesson 2");
}
}  // namespace lesson_02
