#include <cmath>
#include <iostream>
#include <limits>
#include <stdexcept>

#include "exercise.h"
int main() {
  // 中文：本课验收；契约失败结束程序，不把未完成练习算作通过。
  // English: Lesson acceptance; contract failures terminate, including
  // unfinished TODOs.
  std::vector<float> a{2, -3}, b{3, 1};
  pusch::Require(
      lesson_19::CombineHarq(a, b, false) == std::vector<float>({5, -2}),
      "combine");
  pusch::Require(lesson_19::CombineHarq(a, b, true) == b, "new TB must reset");
  std::cout << "PASS lesson 19\n";
  return 0;
}
