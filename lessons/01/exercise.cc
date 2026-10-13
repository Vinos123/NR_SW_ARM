#include "exercise.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <random>
#include <stdexcept>
namespace lesson_01 {

int CountResourceElements(int prbs) {
  // TODO(1): 检查PRB数量必须为正
  // English: Validate positive PRB count
  if (prbs <= 0) throw std::invalid_argument("prbs must be positive.");

  // TODO(2): 计算每个符号的子载波数
  // English: Count subcarriers per symbol
  const int kCountScs = 12 * prbs;
  // TODO(3): 乘以14并返回总RE
  // English: Multiply by 14 and return RE count
  return 14 * kCountScs;
  pusch::Fail("NOT IMPLEMENTED: lesson 1");
}
}  // namespace lesson_01
