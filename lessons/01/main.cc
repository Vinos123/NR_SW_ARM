#include <cmath>
#include <iostream>
#include <limits>
#include <stdexcept>

#include "exercise.h"
int main() {
  // 中文：本课验收；契约失败结束程序，不把未完成练习算作通过。
  // English: Lesson acceptance; contract failures terminate, including
  // unfinished TODOs.
  pusch::Require(lesson_01::CountResourceElements(6) == 1008,
                 "expected 1008 RE");
  std::cout << "PRB=6 total_RE=1008\n";
  std::cout << "PASS lesson 01\n";
  return 0;
}
