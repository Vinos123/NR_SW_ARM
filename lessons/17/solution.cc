#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <random>
#include <stdexcept>

#include "exercise.h"
namespace lesson_17 {

std::vector<Complex> EstimateChannel(const ResourceGrid& grid,
                                     const PuschConfig& c) {
  auto pilots = pusch::Dmrs(c, 2);
  pusch::Require(grid.Subcarriers() == c.prbs * 12, "pilot/grid mismatch");
  std::vector<Complex> channel(grid.Subcarriers());
  for (int k = 0; k < grid.Subcarriers(); k += 2)
    channel[k] = grid.At(2, k) / pilots[k / 2];
  // 中文：频域线性插值、末端保持、时间不变是假设，不是协议要求。
  // English: Linear frequency interpolation, edge hold and static time model.
  for (int k = 1; k < grid.Subcarriers(); k += 2)
    channel[k] = k + 1 < grid.Subcarriers()
                     ? (channel[k - 1] + channel[k + 1]) * 0.5f
                     : channel[k - 1];
  return channel;
}
}  // namespace lesson_17
