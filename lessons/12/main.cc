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
  auto sent = pusch::RateMatch(pusch::LdpcEncode(block, p), p, 1400, 0, 2);
  std::vector<float> llrs;
  for (auto bit : sent) llrs.push_back(bit ? -10.f : 10.f);
  pusch::Require(pusch::LdpcDecoderAdapter().Decode(
                     lesson_12::RateRecover(llrs, p, 0, 2), p) == block.bits,
                 "decode mismatch");
  std::cout << "PASS lesson 12\n";
  return 0;
}
