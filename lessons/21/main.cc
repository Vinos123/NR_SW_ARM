#include <cmath>
#include <iostream>
#include <limits>
#include <stdexcept>

#include "exercise.h"
int main() {
  // 中文：本课验收；契约失败结束程序，不把未完成练习算作通过。
  // English: Lesson acceptance; contract failures terminate, including
  // unfinished TODOs.
  pusch::Require(lesson_21::PuschPowerDbm(23, -80, 1, 120, 6, 1, 0, 0) == 23,
                 "power clipping");
  auto link = pusch::RunLink({}, std::numeric_limits<float>::infinity(), 7);
  pusch::Require(link.crc_ok && link.bit_errors == 0, "full chain");
  std::cout << "PUSCH power capped at 23 dBm; TB CRC PASS\n";
  std::cout << "PASS lesson 21\n";
  return 0;
}
