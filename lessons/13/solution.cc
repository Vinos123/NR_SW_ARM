#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <random>
#include <stdexcept>

#include "exercise.h"
namespace lesson_13 {

int TransportBlockSize(const PuschConfig& c) {
  (void)pusch::TargetRate(c);
  // 中文：38.214 6.1.4.2引用5.1.3.2；本配置无额外开销、单层。
  // English: 38.214 6.1.4.2 references 5.1.3.2; one layer, no overhead.
  float n_info = 156 * c.prbs * c.qm * pusch::TargetRate(c);
  constexpr int kSmallTbs[] = {
      24,   32,   40,   48,   56,   64,   72,   80,   88,   96,   104,  112,
      120,  128,  136,  144,  152,  160,  168,  176,  184,  192,  208,  224,
      240,  256,  272,  288,  304,  320,  336,  352,  368,  384,  408,  432,
      456,  480,  504,  528,  552,  576,  608,  640,  672,  704,  736,  768,
      808,  848,  888,  928,  984,  1032, 1064, 1128, 1160, 1192, 1224, 1256,
      1288, 1320, 1352, 1416, 1480, 1544, 1608, 1672, 1736, 1800, 1864, 1928,
      2024, 2088, 2152, 2216, 2280, 2408, 2472, 2536, 2600, 2664, 2728, 2792,
      2856, 2976, 3104, 3240, 3368, 3496, 3624, 3752, 3824};
  if (n_info <= 3824) {
    int n = std::max(3, static_cast<int>(std::floor(std::log2(n_info))) - 6);
    int quantized = std::max(24, (static_cast<int>(n_info) >> n) << n);
    return *std::lower_bound(std::begin(kSmallTbs), std::end(kSmallTbs),
                             quantized);
  }
  int n = static_cast<int>(std::floor(std::log2(n_info - 24))) - 5;
  int quantized = std::max(
      3840, static_cast<int>(std::round((n_info - 24) / (1 << n))) * (1 << n));
  int blocks = pusch::TargetRate(c) <= 0.25f ? (quantized + 24 + 3815) / 3816
               : quantized > 8424            ? (quantized + 24 + 8423) / 8424
                                             : 1;
  return 8 * blocks * ((quantized + 24 + 8 * blocks - 1) / (8 * blocks)) - 24;
}
}  // namespace lesson_13
