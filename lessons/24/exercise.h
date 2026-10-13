#ifndef PUSCH_LESSON_24_EXERCISE_H_
#define PUSCH_LESSON_24_EXERCISE_H_
#include "pusch/core.h"
#include "pusch/simd.h"
namespace lesson_24 {
using pusch::Bits;
using pusch::BlerResult;
using pusch::CodeBlock;
using pusch::CodingPlan;
using pusch::Complex;
using pusch::LdpcDecoderAdapter;
using pusch::LinkResult;
using pusch::PuschConfig;
using pusch::ResourceGrid;
// 中文：本课练习接口；输入输出与边界说明见本目录README。
// English: Lesson interface; see the local README for contracts and bounds.
bool OptimizedLlr(std::span<const float> iq, float variance,
                  std::span<float> output);
// 中文：单独编译的指令内核，scale为2*sqrt(2)/N0。
// English: Separately compiled kernel; scale is 2*sqrt(2)/N0.
void LessonKernel(const float* input, float* output, size_t count, float scale);
}  // namespace lesson_24
#endif
