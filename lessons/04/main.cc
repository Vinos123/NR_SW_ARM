#include <cmath>
#include <iostream>
#include <limits>
#include <stdexcept>

#include "exercise.h"
int main() {
  // 中文：本课验收；契约失败结束程序，不把未完成练习算作通过。
  // English: Lesson acceptance; contract failures terminate, including
  // unfinished TODOs.
  pusch::Require(lesson_04::Gold(12345, 256) == pusch::Gold(12345, 256),
                 "Gold mismatch");
  auto bits = pusch::MakeBits(64);
  pusch::Require(pusch::Scramble(pusch::Scramble(bits, 12345), 12345) == bits,
                 "XOR inverse");
  std::cout << "c_init=12345 count=256\n";
  std::cout << "PASS lesson 04\n";
  return 0;
}
