#include "exercise.h"
#if defined(PUSCH_LESSON_SIMD)
#include <immintrin.h>
#endif
namespace lesson_23 {
void LessonKernel(const float* input, float* output, size_t count,
                  float scale) {
  // TODO(1): 加载有效元素，乘scale，再写回。
  // English: Load active elements, multiply by scale, then store.
  // TODO(2): 用掩码处理尾部，不能越界访问。
  // English: Handle tails with a mask; never access out of bounds.
  pusch::Fail("NOT IMPLEMENTED: lesson 23 instruction kernel");
}
}  // namespace lesson_23
