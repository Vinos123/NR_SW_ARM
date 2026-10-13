#ifndef PUSCH_SIMD_H_
#define PUSCH_SIMD_H_
#include <span>
#include <string_view>
namespace pusch {
// 中文：输入交织I/Q；每个float对应一个输出LLR，不要求对齐。
// English: Interleaved I/Q; one output LLR per float, no alignment required.
void QpskLlrScalar(std::span<const float> iq, float variance,
                   std::span<float> output);
bool KernelAvailable(std::string_view kernel);
// 中文：返回false表示未编译或硬件不可用；不会偷偷改为标量执行。
// English: false means not built or unsupported; no silent scalar fallback.
bool QpskLlr(std::string_view kernel, std::span<const float> iq, float variance,
             std::span<float> output);
}  // namespace pusch
#endif
