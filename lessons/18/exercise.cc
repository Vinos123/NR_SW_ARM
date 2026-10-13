#include "exercise.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <random>
#include <stdexcept>
namespace lesson_18 {

BlerResult MeasureBler(const PuschConfig& config, float snr_db, int blocks,
                       uint32_t seed) {
  // TODO(1): 逐TB改变随机种子
  // English: Advance seed per TB
  // TODO(2): 按CRC统计失败
  // English: Count CRC failures
  // TODO(3): 计算Wilson区间
  // English: Compute Wilson interval
  // TODO(4): 用CLI导出SNR扫描
  // English: Export SNR sweep
  pusch::Fail("NOT IMPLEMENTED: lesson 18");
}
}  // namespace lesson_18
