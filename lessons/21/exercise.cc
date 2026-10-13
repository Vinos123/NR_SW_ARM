#include "exercise.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <random>
#include <stdexcept>
namespace lesson_21 {

float PuschPowerDbm(float p_max, float p0, float alpha, float path_loss,
                    int prbs, int mu, float delta_tf, float adjustment) {
  // TODO(1): 计算带宽项10log10(2^mu*M)
  // English: Compute bandwidth term
  // TODO(2): 加入路径损耗补偿
  // English: Add path-loss compensation
  // TODO(3): 执行PCMAX限幅
  // English: Apply PCMAX cap
  // TODO(4): 逐层跟踪完整链路长度
  // English: Trace complete chain lengths
  pusch::Fail("NOT IMPLEMENTED: lesson 21");
}
}  // namespace lesson_21
