#include <immintrin.h>

#include <cstddef>
namespace pusch {
void QpskAvx512(const float* input, float* output, size_t count, float scale) {
  __m512 factor = _mm512_set1_ps(scale);
  size_t i = 0;
  for (; i + 16 <= count; i += 16)
    _mm512_storeu_ps(output + i,
                     _mm512_mul_ps(_mm512_loadu_ps(input + i), factor));
  // 中文：掩码禁止访问数组末尾之外的元素，覆盖任意尾部长度。
  // English: Mask prevents out-of-bounds accesses for any tail length.
  if (i < count) {
    __mmask16 mask = static_cast<__mmask16>((1U << (count - i)) - 1);
    auto tail = _mm512_maskz_loadu_ps(mask, input + i);
    _mm512_mask_storeu_ps(output + i, mask, _mm512_mul_ps(tail, factor));
  }
}
}  // namespace pusch
