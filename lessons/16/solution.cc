#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <random>
#include <stdexcept>

#include "exercise.h"
namespace lesson_16 {

std::vector<Complex> ApplyChannel(std::span<const Complex> input,
                                  std::span<const Complex> taps) {
  pusch::Require(!taps.empty(), "empty channel");
  std::vector<Complex> output(input.size(), 0);
  for (size_t i = 0; i < input.size(); ++i)
    for (size_t j = 0; j < taps.size() && j <= i; ++j)
      output[i] += taps[j] * input[i - j];
  return output;
}
}  // namespace lesson_16
