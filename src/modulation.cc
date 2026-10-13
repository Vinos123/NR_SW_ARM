#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <limits>
#include <random>

#include "pusch/core.h"

namespace pusch {
[[noreturn]] void Fail(const char* message) {
  std::fprintf(stderr, "%s\n", message);
  std::exit(1);
}
void Require(bool condition, const char* message) {
  if (!condition) Fail(message);
}
Bits MakeBits(int count, uint32_t seed) {
  Require(count >= 0, "negative bit count");
  std::mt19937 generator(seed);
  Bits bits(count);
  for (auto& bit : bits) bit = generator() & 1U;
  return bits;
}
Bits Gold(uint32_t c_init, int count) {
  Require(count >= 0 && c_init < (1U << 31), "invalid Gold parameters");
  // 中文：先推进Nc=1600，再输出x1(n+Nc) XOR x2(n+Nc)。
  // English: Skip Nc=1600; output x1(n+Nc) XOR x2(n+Nc).
  // Reference: TS 38.211 V17.5.0, 5.2.1.
  constexpr int kNc = 1600;
  Bits x1(count + kNc + 31, 0), x2(x1.size(), 0), result(count);
  x1[0] = 1;
  for (int i = 0; i < 31; ++i) x2[i] = (c_init >> i) & 1U;
  for (int i = 0; i < count + kNc; ++i) {
    x1[i + 31] = x1[i + 3] ^ x1[i];
    x2[i + 31] = x2[i + 3] ^ x2[i + 2] ^ x2[i + 1] ^ x2[i];
  }
  for (int i = 0; i < count; ++i) result[i] = x1[i + kNc] ^ x2[i + kNc];
  return result;
}
Bits Scramble(std::span<const uint8_t> bits, uint32_t c_init) {
  auto output = Gold(c_init, static_cast<int>(bits.size()));
  for (size_t i = 0; i < bits.size(); ++i) {
    Require(bits[i] <= 1, "non-binary input");
    output[i] ^= bits[i];
  }
  return output;
}
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
std::vector<Complex> AddAwgn(std::span<const Complex> input, float variance,
                             uint32_t seed) {
  Require(variance >= 0, "negative noise variance");
  std::mt19937 generator(seed);
  std::normal_distribution<float> gaussian(0, std::sqrt(variance / 2));
  std::vector<Complex> output(input.begin(), input.end());
  // 中文：I/Q各占一半功率；标准库正态分布不保证跨实现逐样本一致。
  // English: I/Q each have half the power; Gaussian samples are not portable.
  for (auto& value : output) {
    float re = gaussian(generator), im = gaussian(generator);
    value += Complex(re, im);
  }
  return output;
}
}  // namespace pusch
