#include "exercise.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <random>
#include <stdexcept>
namespace lesson_19 {

std::vector<float> CombineHarq(std::span<const float> previous,
                               std::span<const float> current, bool new_data) {
  // TODO(1): 新数据直接覆盖
  // English: Replace on new data
  // TODO(2): 检查重传缓冲长度
  // English: Check retransmission size
  // TODO(3): 逐元素相加
  // English: Add LLRs
  // TODO(4): 用RV0与RV2运行核心HARQ测试
  // English: Run the RV0/RV2 core test
  pusch::Fail("NOT IMPLEMENTED: lesson 19");
}
}  // namespace lesson_19
