#include <cmath>
#include <random>

#include "pusch/core.h"

namespace pusch {
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
