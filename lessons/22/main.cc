#include <cmath>
#include <iostream>
#include <limits>
#include <stdexcept>

#include "exercise.h"
int main() {
  // 中文：本课验收；契约失败结束程序，不把未完成练习算作通过。
  // English: Lesson acceptance; contract failures terminate, including
  // unfinished TODOs.
  std::vector<float> iq{.70710678f, -.70710678f, .25f}, out(3);
  lesson_22::QpskLlrScalar(iq, .5f, out);
  pusch::Require(std::abs(out[0] - 4) < 1e-5f && std::abs(out[1] + 4) < 1e-5f,
                 "scalar LLR");
  std::cout << "PASS lesson 22\n";
  return 0;
}
