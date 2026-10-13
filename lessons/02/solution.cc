#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <random>
#include <stdexcept>

#include "exercise.h"
namespace lesson_02 {

int GridIndex(int symbol, int subcarrier, int tones) {
  pusch::Require(tones > 0 && symbol >= 0 && symbol < 14 && subcarrier >= 0 &&
                     subcarrier < tones,
                 "invalid grid index");
  // 中文：先跨越完整OFDM符号，再加当前符号内的子载波偏移。
  // English: Skip complete OFDM symbols, then add the subcarrier offset.
  return symbol * tones + subcarrier;
}
}  // namespace lesson_02
