#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <random>
#include <stdexcept>

#include "exercise.h"
namespace lesson_20 {

Bits MultiplexToy(std::span<const uint8_t> data, std::span<const uint8_t> uci) {
  pusch::Require(data.size() == 4 && uci.size() == 2,
                 "toy requires 4 data + 2 UCI bits");
  // 中文：仅演示索引预算；位置[1,4]是人为选择，不是NR UCI算法。
  // English: Toy index budget only; positions [1,4] are NOT NR UCI mapping.
  return {data[0], uci[0], data[1], data[2], uci[1], data[3]};
}
}  // namespace lesson_20
