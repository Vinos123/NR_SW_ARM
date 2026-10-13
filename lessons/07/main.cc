#include <cmath>
#include <iostream>
#include <limits>
#include <stdexcept>

#include "exercise.h"
int main() {
  // 中文：本课验收；契约失败结束程序，不把未完成练习算作通过。
  // English: Lesson acceptance; contract failures terminate, including
  // unfinished TODOs.
  auto bits = pusch::MakeBits(100);
  pusch::Require(lesson_07::CalculateCrc(bits, 16, 0x1021) ==
                     pusch::CalculateCrc(bits, 16, 0x1021),
                 "CRC value");
  auto attached = pusch::AttachCrc(bits, 16, 0x1021);
  pusch::Require(lesson_07::CalculateCrc(attached, 16, 0x1021) == 0,
                 "CRC residual");
  std::cout << "PASS lesson 07\n";
  return 0;
}
