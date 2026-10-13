#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <random>
#include <stdexcept>

#include "exercise.h"
namespace lesson_21 {

float PuschPowerDbm(float p_max, float p0, float alpha, float path_loss,
                    int prbs, int mu, float delta_tf, float adjustment) {
  pusch::Require(prbs > 0 && mu >= 0 && mu <= 4 && alpha >= 0 && alpha <= 1,
                 "invalid power parameters");
  // 中文：单载波简化场景；没有实现多信道同时发射的功率优先级。
  // English: Single-carrier simplification; no simultaneous-channel priority.
  // Reference: TS 38.213 V17.6.0, 7.1.1.
  return std::min(p_max,
                  p0 + 10.f * std::log10(static_cast<float>(prbs * (1 << mu))) +
                      alpha * path_loss + delta_tf + adjustment);
}
}  // namespace lesson_21
