#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <random>
#include <stdexcept>

#include "exercise.h"
namespace lesson_22 {

void QpskLlrScalar(std::span<const float> iq, float variance,
                   std::span<float> output) {
  pusch::Require(iq.size() == output.size() && variance > 0,
                 "invalid QPSK spans/noise");
  // 中文：精确QPSK LLR为2*sqrt(2)*I/N0及2*sqrt(2)*Q/N0。
  // English: Exact QPSK LLR is 2*sqrt(2)*I/N0 and 2*sqrt(2)*Q/N0.
  float scale = 2.f * std::sqrt(2.f) / variance;
  for (size_t i = 0; i < iq.size(); ++i) output[i] = iq[i] * scale;
}
}  // namespace lesson_22
