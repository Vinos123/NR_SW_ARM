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
  auto data = pusch::Modulate(pusch::MakeBits(pusch::DataReCount(c) * 2), 2);
  auto grid = lesson_14::MapGrid(data, c);
  pusch::Require(pusch::ExtractData(grid) == data, "mapping order");
  pusch::Require(grid.At(2, 0) == pusch::Dmrs(c, 2)[0], "pilot position");
  std::cout << "PASS lesson 14\n";
  return 0;
}
