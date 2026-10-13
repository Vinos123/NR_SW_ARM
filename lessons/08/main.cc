#include <cmath>
#include <iostream>
#include <limits>
#include <stdexcept>

#include "exercise.h"
int main() {
  // 中文：本课验收；契约失败结束程序，不把未完成练习算作通过。
  // English: Lesson acceptance; contract failures terminate, including
  // unfinished TODOs.
  pusch::Require(lesson_08::SelectBaseGraph(292, .9f) == 2, "A boundary");
  pusch::Require(lesson_08::SelectBaseGraph(293, .9f) == 1, "A boundary +1");
  pusch::Require(lesson_08::SelectBaseGraph(4000, .25f) == 2, "rate boundary");
  auto p = pusch::MakeCodingPlan(9000, .7f);
  pusch::Require(p.c == 2, "two code blocks");
  std::cout << "PASS lesson 08\n";
  return 0;
}
