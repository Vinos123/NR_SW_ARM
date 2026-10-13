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
  auto grid = pusch::MapGrid(data, c);
  auto wave = lesson_15::OfdmModulate(grid);
  pusch::Require(wave.size() == 3840, "slot sample count");
  auto recovered = pusch::ExtractData(pusch::OfdmDemodulate(wave, c.prbs));
  for (size_t i = 0; i < data.size(); ++i)
    pusch::Require(std::abs(data[i] - recovered[i]) < 1e-5f, "FFT roundtrip");
  std::cout << "PASS lesson 15\n";
  return 0;
}
