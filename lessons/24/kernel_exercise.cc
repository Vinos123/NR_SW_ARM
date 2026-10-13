#include "exercise.h"
#if defined(PUSCH_LESSON_SIMD)
#include <arm_sve.h>
#endif
namespace lesson_24 {
void LessonKernel(const float* input, float* output, size_t count,
                  float scale) {
  // TODO(1): 加载有效元素，乘scale，再写回。
  // English: Load active elements, multiply by scale, then store.
  // TODO(2): 用谓词处理尾部，不能越界访问。
  // English: Handle tails with a predicate; never access out of bounds.
  pusch::Fail("NOT IMPLEMENTED: lesson 24 instruction kernel");
}
}  // namespace lesson_24
