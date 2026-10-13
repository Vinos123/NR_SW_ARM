#include "pusch/simd.h"

#include <cmath>

#include "pusch/core.h"
#if defined(PUSCH_HAS_SVE)
#include <asm/hwcap.h>
#include <sys/auxv.h>
#endif
namespace pusch {
#if defined(PUSCH_HAS_AVX512)
void QpskAvx512(const float*, float*, size_t, float);
#endif
#if defined(PUSCH_HAS_SVE)
void QpskSve(const float*, float*, size_t, float);
#endif
void QpskLlrScalar(std::span<const float> iq, float variance,
                   std::span<float> output) {
  Require(iq.size() == output.size() && variance > 0,
          "invalid QPSK spans/noise");
  // 中文：精确QPSK LLR为2*sqrt(2)*I/N0及2*sqrt(2)*Q/N0。
  // English: Exact QPSK LLR is 2*sqrt(2)*I/N0 and 2*sqrt(2)*Q/N0.
  float scale = 2.f * std::sqrt(2.f) / variance;
  for (size_t i = 0; i < iq.size(); ++i) output[i] = iq[i] * scale;
}
bool KernelAvailable(std::string_view kernel) {
  if (kernel == "scalar") return true;
#if defined(PUSCH_HAS_AVX512)
  if (kernel == "avx512") {
    __builtin_cpu_init();
    return __builtin_cpu_supports("avx512f");
  }
#endif
#if defined(PUSCH_HAS_SVE)
  if (kernel == "sve") return (getauxval(AT_HWCAP) & HWCAP_SVE) != 0;
#endif
  return false;
}
bool QpskLlr(std::string_view kernel, std::span<const float> iq, float variance,
             std::span<float> output) {
  Require(iq.size() == output.size() && variance > 0,
          "invalid QPSK spans/noise");
  if (!KernelAvailable(kernel)) return false;
  if (kernel == "scalar") {
    QpskLlrScalar(iq, variance, output);
    return true;
  }
  [[maybe_unused]] float scale = 2.f * std::sqrt(2.f) / variance;
#if defined(PUSCH_HAS_AVX512)
  if (kernel == "avx512")
    QpskAvx512(iq.data(), output.data(), iq.size(), scale);
#endif
#if defined(PUSCH_HAS_SVE)
  if (kernel == "sve") QpskSve(iq.data(), output.data(), iq.size(), scale);
#endif
  return true;
}
}  // namespace pusch
