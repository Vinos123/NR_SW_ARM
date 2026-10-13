#include <cmath>
#include <iostream>
#include <limits>
#include <stdexcept>

#include "exercise.h"
int main() {
  // 中文：本课验收；契约失败结束程序，不把未完成练习算作通过。
  // English: Lesson acceptance; contract failures terminate, including
  // unfinished TODOs.
  pusch::Require(lesson_02::GridIndex(2, 4, 72) == 148, "index must be 148");
  pusch::ResourceGrid grid(6);
  grid.At(2, 4) = {1, 0};
  pusch::Require(grid.At(2, 4) == pusch::Complex(1, 0), "grid mismatch");
  std::cout << "PASS lesson 02\n";
  return 0;
}
