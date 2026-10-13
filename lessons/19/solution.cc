#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <random>
#include <stdexcept>

#include "exercise.h"
namespace lesson_19 {

std::vector<float> CombineHarq(std::span<const float> previous,
                               std::span<const float> current, bool new_data) {
  // 中文：新TB必须丢弃旧LLR；重传只能合并同一TB、同一编码计划。
  // English: New TB discards old LLRs; retransmissions require the same
  // TB/plan.
  if (new_data) return {current.begin(), current.end()};
  pusch::Require(previous.size() == current.size(),
                 "HARQ buffer size mismatch");
  std::vector<float> combined(current.size());
  for (size_t i = 0; i < current.size(); ++i)
    combined[i] = previous[i] + current[i];
  return combined;
}
}  // namespace lesson_19
