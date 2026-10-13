#include "profile.h"
#include "pusch/core.h"

namespace pusch {
std::vector<Complex> Dmrs(const PuschConfig& c, int symbol) {
  internal::ValidateProfile(c);
  Require(symbol == 2, "profile DM-RS symbol must be 2");
  // 中文：nSCID=0、port0、参考PRB起点0；c_init使用64位避免乘法溢出。
  // English: nSCID=0, port0, reference PRB start 0; use 64-bit c_init math.
  // Reference: TS 38.211 V17.5.0, 6.4.1.1.1.
  uint64_t init =
      (uint64_t{1} << 17) * (14 * c.slot + symbol + 1) * (2 * c.nid + 1) +
      2 * c.nid;
  return Modulate(Gold(init & 0x7fffffffU, c.prbs * 12), 2);
}
}  // namespace pusch
