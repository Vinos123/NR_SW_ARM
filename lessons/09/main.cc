#include <cmath>
#include <iostream>
#include <limits>
#include <stdexcept>

#include "exercise.h"
int main() {
  // 中文：本课验收；契约失败结束程序，不把未完成练习算作通过。
  // English: Lesson acceptance; contract failures terminate, including
  // unfinished TODOs.
  auto p = lesson_09::MakeCodingPlan(24, .5f);
  pusch::Require(p.bg == 2 && p.b == 40 && p.kb == 6 && p.z == 7 && p.k == 70 &&
                     p.filler == 30,
                 "small BG2");
  std::cout << "A=" << p.a << " B=" << p.b << " Zc=" << p.z << " K=" << p.k
            << " F=" << p.filler << '\n';
  std::cout << "PASS lesson 09\n";
  return 0;
}
