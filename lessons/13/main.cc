#include <cmath>
#include <iostream>
#include <limits>
#include <stdexcept>

#include "exercise.h"
int main() {
  // 中文：本课验收；契约失败结束程序，不把未完成练习算作通过。
  // English: Lesson acceptance; contract failures terminate, including
  // unfinished TODOs.
  pusch::PuschConfig c;
  pusch::Require(lesson_13::TransportBlockSize(c) == 576,
                 "default TBS must be 576");
  pusch::Require(pusch::DataReCount(c) == 972, "actual RE count");
  std::cout << "TBS=576 G=1944 actual_data_RE=972\n";
  std::cout << "PASS lesson 13\n";
  return 0;
}
