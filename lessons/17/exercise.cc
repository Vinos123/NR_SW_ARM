#include "exercise.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <random>
#include <stdexcept>
namespace lesson_17 {

std::vector<Complex> EstimateChannel(const ResourceGrid& grid,
                                     const PuschConfig& c) {
  // TODO(1): 导频相除求LS
  // English: Divide received pilots by references
  // TODO(2): 频域线性插值
  // English: Interpolate in frequency
  // TODO(3): 末端使用保持
  // English: Hold the final edge
  // TODO(4): 阅读Equalize并推导输出噪声
  // English: Derive equalizer noise
  pusch::Fail("NOT IMPLEMENTED: lesson 17");
}
}  // namespace lesson_17
