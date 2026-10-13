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
  auto grid = pusch::MapGrid(
      pusch::Modulate(pusch::MakeBits(pusch::DataReCount(c) * 2), 2), c);
  pusch::Complex h(.8f, .2f);
  for (int l = 0; l < 14; ++l)
    for (int k = 0; k < grid.Subcarriers(); ++k) grid.At(l, k) *= h;
  for (auto v : lesson_17::EstimateChannel(grid, c))
    pusch::Require(std::abs(v - h) < 1e-6f, "LS estimate");
  std::cout << "PASS lesson 17\n";
  return 0;
}
