#include <cmath>
#include <iostream>
#include <limits>
#include <stdexcept>

#include "exercise.h"
int main() {
  // 中文：本课验收；契约失败结束程序，不把未完成练习算作通过。
  // English: Lesson acceptance; contract failures terminate, including
  // unfinished TODOs.
  auto r =
      lesson_18::MeasureBler({}, std::numeric_limits<float>::infinity(), 3, 1);
  pusch::Require(r.errors == 0 && r.high > 0 && r.low >= 0, "Wilson interval");
  std::cout << "errors=" << r.errors << " CI95=[" << r.low << ',' << r.high
            << "]\n";
  std::cout << "PASS lesson 18\n";
  return 0;
}
