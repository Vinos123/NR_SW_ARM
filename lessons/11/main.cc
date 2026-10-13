#include <cmath>
#include <iostream>
#include <limits>
#include <stdexcept>

#include "exercise.h"
int main() {
  // 中文：本课验收；契约失败结束程序，不把未完成练习算作通过。
  // English: Lesson acceptance; contract failures terminate, including
  // unfinished TODOs.
  auto p = pusch::MakeCodingPlan(552, .5f);
  auto full = pusch::LdpcEncode(pusch::Segment(pusch::MakeBits(552), p)[0], p);
  for (int rv = 0; rv < 4; ++rv)
    pusch::Require(lesson_11::RateMatch(full, p, 1200, rv, 4) ==
                       pusch::RateMatch(full, p, 1200, rv, 4),
                   "RV mismatch");
  std::cout << "PASS lesson 11\n";
  return 0;
}
