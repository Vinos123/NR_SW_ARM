#ifndef PUSCH_SRC_PROFILE_H_
#define PUSCH_SRC_PROFILE_H_
#include "pusch/core.h"
namespace pusch::internal {
// 中文：共享教学配置验证，不依赖FFT或信道编码。
// English: Shared teaching-profile validation; no FFT or coding dependency.
inline void ValidateProfile(const PuschConfig& c) {
  Require(c.prbs > 0 && c.prbs <= 20 && c.slot >= 0 && c.slot < 20,
          "profile requires 1..20 PRBs and slot 0..19");
  Require(c.mcs >= 0 && c.mcs <= 16 && c.qm == (c.mcs < 10 ? 2 : 4),
          "MCS/Qm mismatch for table 5.1.3.1-1 subset");
  Require(c.nid <= 1007 && c.rv >= 0 && c.rv < 4, "invalid identity/RV");
}
}  // namespace pusch::internal
#endif  // PUSCH_SRC_PROFILE_H_
