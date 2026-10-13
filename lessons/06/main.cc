#include <cmath>
#include <iostream>
#include <limits>
#include <stdexcept>

#include "exercise.h"
int main() {
  // 中文：本课验收；契约失败结束程序，不把未完成练习算作通过。
  // English: Lesson acceptance; contract failures terminate, including
  // unfinished TODOs.
  auto symbols = pusch::Modulate(pusch::Bits{0, 0, 1, 1}, 2);
  auto soft = lesson_06::Demodulate(symbols, 2, .5f);
  pusch::Require(std::abs(soft[0] - 4.f) < 1e-5f && soft[2] < 0,
                 "LLR sign/scale");
  std::cout << "PASS lesson 06\n";
  return 0;
}
