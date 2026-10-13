#include <cmath>
#include <iostream>
#include <limits>
#include <stdexcept>

#include "exercise.h"
int main() {
  // 中文：本课验收；契约失败结束程序，不把未完成练习算作通过。
  // English: Lesson acceptance; contract failures terminate, including
  // unfinished TODOs.
  pusch::Require(
      lesson_20::MultiplexToy(pusch::Bits{0, 0, 0, 0}, pusch::Bits{1, 1}) ==
          pusch::Bits({0, 1, 0, 0, 1, 0}),
      "toy positions");
  auto x = pusch::Modulate(pusch::MakeBits(24), 2);
  auto y = pusch::Transform(pusch::Transform(x, false), true);
  for (size_t i = 0; i < x.size(); ++i)
    pusch::Require(std::abs(x[i] - y[i]) < 1e-5f, "DFT inverse");
  std::cout << "PASS lesson 20\n";
  return 0;
}
