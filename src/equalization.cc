#include "pusch/core.h"

namespace pusch {
std::vector<Complex> Equalize(std::span<const Complex> data,
                              std::span<const Complex> channel, float variance,
                              bool mmse) {
  Require(data.size() == channel.size() && variance >= 0,
          "equalizer size/noise");
  std::vector<Complex> output(data.size());
  for (size_t i = 0; i < data.size(); ++i) {
    float denominator = std::norm(channel[i]) + (mmse ? variance : 0.f);
    Require(denominator > 0, "zero channel: equalization undefined");
    output[i] = data[i] * std::conj(channel[i]) / denominator;
  }
  return output;
}
}  // namespace pusch
