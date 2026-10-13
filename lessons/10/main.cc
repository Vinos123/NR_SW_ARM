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
  auto block = pusch::Segment(pusch::MakeBits(552), p)[0];
  auto cw = lesson_10::LdpcEncode(block, p);
  pusch::Require(pusch::CheckParity(cw, p), "H*c must be zero");
  pusch::Require(cw == pusch::LdpcEncode(block, p), "reference mismatch");
  std::cout << "PASS lesson 10\n";
  return 0;
}
