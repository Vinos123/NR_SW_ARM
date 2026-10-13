#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <random>
#include <stdexcept>

#include "exercise.h"
namespace lesson_08 {

int SelectBaseGraph(int a, float rate) {
  pusch::Require(a > 0 && rate > 0 && rate <= 1, "invalid A/R");
  // 中文：三项BG2条件为逻辑或，中间一项包含A与R两个条件。
  // English: OR the three BG2 rules; the middle rule combines A and R.
  // Reference: TS 38.212 V17.6.0, 6.2.2.
  return a <= 292 || (a <= 3824 && rate <= 0.67f) || rate <= 0.25f ? 2 : 1;
}
}  // namespace lesson_08
