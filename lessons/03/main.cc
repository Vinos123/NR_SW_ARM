#include <cmath>
#include <iostream>
#include <limits>
#include <stdexcept>

#include "exercise.h"
int main() {
  // 中文：本课验收；契约失败结束程序，不把未完成练习算作通过。
  // English: Lesson acceptance; contract failures terminate, including
  // unfinished TODOs.
  pusch::Require(lesson_03::MakeBits(16, 7) == lesson_03::MakeBits(16, 7),
                 "seed reproducibility");
  pusch::Require(lesson_03::MakeBits(16, 7) != lesson_03::MakeBits(16, 8),
                 "seed differentiation");
  pusch::Require(lesson_03::MakeBits(0, 7).empty(), "zero length");
  std::cout << "PASS lesson 03\n";
  return 0;
}
