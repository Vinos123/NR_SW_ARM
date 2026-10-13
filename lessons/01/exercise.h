#ifndef PUSCH_LESSON_01_EXERCISE_H_
#define PUSCH_LESSON_01_EXERCISE_H_
#include "pusch/core.h"
#include "pusch/simd.h"
namespace lesson_01 {
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
int CountResourceElements(int prbs);
}  // namespace lesson_01
#endif
