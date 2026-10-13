#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <random>
#include <stdexcept>

#include "exercise.h"
namespace lesson_09 {
int DivideUp(int value, int divisor) { return (value + divisor - 1) / divisor; }
CodingPlan MakeCodingPlan(int a, float rate) {
  CodingPlan p;
  p.a = a;
  p.bg = pusch::SelectBaseGraph(a, rate);
  p.b = a + (a > 3824 ? 24 : 16);
  int kcb = p.bg == 1 ? 8448 : 3840;
  p.c = p.b <= kcb ? 1 : DivideUp(p.b, kcb - 24);
  p.cb_crc = p.c == 1 ? 0 : 24;
  // 中文：主线输入为合法TBS；拒绝不能均分的任意长度，避免隐式补位。
  // English: Require valid equally divisible TB sizes; no implicit padding.
  pusch::Require(p.b % p.c == 0, "B must be divisible by C in this profile");
  p.k_prime = p.b / p.c + p.cb_crc;
  p.kb = p.bg == 1 ? 22 : p.b > 640 ? 10 : p.b > 560 ? 9 : p.b > 192 ? 8 : 6;
  p.z = 0;
  for (int z : {2,   3,   4,   5,   6,   7,   8,   9,   10,  11,  12,  13,  14,
                15,  16,  18,  20,  22,  24,  26,  28,  30,  32,  36,  40,  44,
                48,  52,  56,  60,  64,  72,  80,  88,  96,  104, 112, 120, 128,
                144, 160, 176, 192, 208, 224, 240, 256, 288, 320, 352, 384}) {
    if (p.kb * z >= p.k_prime) {
      p.z = z;
      break;
    }
  }
  pusch::Require(p.z != 0, "no lifting size found");
  // 中文：BG2即使用较小Kb选择Zc，实际信息区长度仍是10*Zc。
  // English: BG2 uses 10*Zc information bits even if selection Kb is smaller.
  // Reference: TS 38.212 V17.6.0, 5.2.2, Table 5.3.2-1.
  p.k = (p.bg == 1 ? 22 : 10) * p.z;
  p.n = (p.bg == 1 ? 66 : 50) * p.z;
  p.filler = p.k - p.k_prime;
  return p;
}
}  // namespace lesson_09
