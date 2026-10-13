#include <cmath>
#include <iostream>
#include <limits>
#include <stdexcept>

#include "exercise.h"
int main() {
  // 中文：本课验收；契约失败结束程序，不把未完成练习算作通过。
  // English: Lesson acceptance; contract failures terminate, including
  // unfinished TODOs.
  for (int qm : {2, 4}) {
    auto bits = pusch::MakeBits(64);
    auto symbols = lesson_05::Modulate(bits, qm);
    pusch::Require(
        pusch::HardDecide(pusch::Demodulate(symbols, qm, .1f)) == bits,
        "modulation mismatch");
  }
  auto z = lesson_05::Modulate(pusch::Bits{0, 0}, 2)[0];
  pusch::Require(std::abs(z - pusch::Complex(.70710678f, .70710678f)) < 1e-6f,
                 "QPSK 00");
  std::cout << "PASS lesson 05\n";
  return 0;
}
