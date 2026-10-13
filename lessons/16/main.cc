#include <cmath>
#include <iostream>
#include <limits>
#include <stdexcept>

#include "exercise.h"
int main() {
  // 中文：本课验收；契约失败结束程序，不把未完成练习算作通过。
  // English: Lesson acceptance; contract failures terminate, including
  // unfinished TODOs.
  std::vector<pusch::Complex> x{{1, 0}, {2, 0}, {3, 0}}, h{{1, 0}, {.5f, 0}};
  auto y = lesson_16::ApplyChannel(x, h);
  pusch::Require(y == std::vector<pusch::Complex>({{1, 0}, {2.5f, 0}, {4, 0}}),
                 "convolution");
  std::cout << "PASS lesson 16\n";
  return 0;
}
