#include <cmath>
#include <cstdlib>
#include <memory>

#include "kiss_fft.h"
#include "pusch/core.h"

namespace pusch {
std::vector<Complex> Transform(std::span<const Complex> input, bool inverse) {
  Require(!input.empty(), "empty FFT");
  auto* cfg =
      kiss_fft_alloc(static_cast<int>(input.size()), inverse, nullptr, nullptr);
  Require(cfg != nullptr, "FFT allocation failed");
  std::unique_ptr<void, decltype(&std::free)> owner(cfg, &std::free);
  std::vector<kiss_fft_cpx> in(input.size()), out(input.size());
  for (size_t i = 0; i < input.size(); ++i)
    in[i] = {input[i].real(), input[i].imag()};
  kiss_fft(cfg, in.data(), out.data());
  // 中文：采用酉归一化，两方向均除sqrt(N)，保持能量和噪声功率。
  // English: Unitary scaling divides both directions by sqrt(N).
  std::vector<Complex> result(input.size());
  float scale = 1.f / std::sqrt(static_cast<float>(input.size()));
  for (size_t i = 0; i < input.size(); ++i)
    result[i] = Complex(out[i].r, out[i].i) * scale;
  return result;
}
}  // namespace pusch
