#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <random>
#include <stdexcept>

#include "exercise.h"
namespace lesson_05 {

std::vector<Complex> Modulate(std::span<const uint8_t> bits, int qm) {
  pusch::Require(qm == 2 || qm == 4, "only QPSK and 16QAM supported");
  pusch::Require(bits.size() % qm == 0, "incomplete symbol");
  std::vector<Complex> output;
  for (size_t i = 0; i < bits.size(); i += qm) {
    for (int j = 0; j < qm; ++j)
      pusch::Require(bits[i + j] <= 1, "invalid bit");
    // 中文：I/Q分别取偶/奇位；16QAM归一化因子为sqrt(10)。
    // English: Even/odd bits select I/Q; 16QAM normalization is sqrt(10).
    // Reference: TS 38.211 V17.5.0, 5.1.3, 5.1.4.
    float re = 1 - 2 * bits[i], im = 1 - 2 * bits[i + 1];
    if (qm == 4) {
      re *= 2 - (1 - 2 * bits[i + 2]);
      im *= 2 - (1 - 2 * bits[i + 3]);
    }
    output.emplace_back(re / std::sqrt(qm == 2 ? 2.f : 10.f),
                        im / std::sqrt(qm == 2 ? 2.f : 10.f));
  }
  return output;
}
}  // namespace lesson_05
