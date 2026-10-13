#include "pusch/core.h"

namespace pusch {
std::vector<Complex> ApplyChannel(std::span<const Complex> input,
                                  std::span<const Complex> taps) {
  Require(!taps.empty(), "empty channel");
  std::vector<Complex> output(input.size(), 0);
  for (size_t i = 0; i < input.size(); ++i)
    for (size_t j = 0; j < taps.size() && j <= i; ++j)
      output[i] += taps[j] * input[i - j];
  return output;
}
}  // namespace pusch
