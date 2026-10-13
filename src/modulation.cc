#include <algorithm>
#include <cmath>
#include <limits>

#include "pusch/core.h"

namespace pusch {
std::vector<Complex> Modulate(std::span<const uint8_t> bits, int qm) {
  Require(qm == 2 || qm == 4, "only QPSK and 16QAM supported");
  Require(bits.size() % qm == 0, "incomplete symbol");
  std::vector<Complex> output;
  for (size_t i = 0; i < bits.size(); i += qm) {
    for (int j = 0; j < qm; ++j) Require(bits[i + j] <= 1, "invalid bit");
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

std::vector<float> Demodulate(std::span<const Complex> symbols, int qm,
                              float noise_variance) {
  Require((qm == 2 || qm == 4) && noise_variance > 0,
          "invalid demodulation parameters");
  std::vector<Complex> constellation;
  std::vector<Bits> labels;
  for (int word = 0; word < (1 << qm); ++word) {
    Bits bits(qm);
    for (int j = 0; j < qm; ++j) bits[j] = (word >> (qm - 1 - j)) & 1;
    constellation.push_back(Modulate(bits, qm)[0]);
    labels.push_back(bits);
  }
  std::vector<float> output;
  for (auto symbol : symbols) {
    for (int bit = 0; bit < qm; ++bit) {
      float d[2] = {std::numeric_limits<float>::infinity(),
                    std::numeric_limits<float>::infinity()};
      for (size_t i = 0; i < labels.size(); ++i)
        d[labels[i][bit]] =
            std::min(d[labels[i][bit]], std::norm(symbol - constellation[i]));
      // 中文：max-log近似属于接收算法，标准未规定此实现。
      // English: Max-log is a receiver choice, not a mandated algorithm.
      output.push_back((d[1] - d[0]) / noise_variance);
    }
  }
  return output;
}

Bits HardDecide(std::span<const float> llrs) {
  Bits output;
  for (float value : llrs) output.push_back(value < 0);
  return output;
}
}  // namespace pusch
