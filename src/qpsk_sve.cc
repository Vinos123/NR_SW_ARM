#include <arm_sve.h>

#include <cstddef>
#include <cstdint>
namespace pusch {
void QpskSve(const float* input, float* output, size_t count, float scale) {
  // 中文：每轮查询运行时向量长度，whilelt谓词同时处理最后不足一向量的数据。
  // English: Runtime vector length plus whilelt handles the final partial
  // vector.
  for (size_t i = 0; i < count; i += svcntw()) {
    svbool_t active =
        svwhilelt_b32(static_cast<uint64_t>(i), static_cast<uint64_t>(count));
    auto values = svld1_f32(active, input + i);
    svst1_f32(active, output + i, svmul_n_f32_x(active, values, scale));
  }
}
}  // namespace pusch
