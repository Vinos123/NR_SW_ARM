#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <random>
#include <stdexcept>

#include "exercise.h"
namespace lesson_06 {

std::vector<float> Demodulate(std::span<const Complex> symbols, int qm,
                              float noise_variance) {
  pusch::Require((qm == 2 || qm == 4) && noise_variance > 0,
                 "invalid demodulation parameters");
  std::vector<Complex> constellation;
  std::vector<Bits> labels;
  for (int word = 0; word < (1 << qm); ++word) {
    Bits bits(qm);
    for (int j = 0; j < qm; ++j) bits[j] = (word >> (qm - 1 - j)) & 1;
    constellation.push_back(pusch::Modulate(bits, qm)[0]);
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
}  // namespace lesson_06
