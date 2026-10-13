#include "exercise.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <random>
#include <stdexcept>
namespace lesson_14 {

ResourceGrid MapGrid(std::span<const Complex> symbols, const PuschConfig& c) {
  // TODO(1): 计算DM-RS初始化并调用Gold
  // English: Initialize DM-RS sequence
  // TODO(2): 标记l=2偶数k
  // English: Mark pilot REs
  // TODO(3): 按k后l递增填充数据
  // English: Fill frequency then time
  // TODO(4): 提取并对比原数据
  // English: Extract and compare
  pusch::Fail("NOT IMPLEMENTED: lesson 14");
}
}  // namespace lesson_14
