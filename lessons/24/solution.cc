#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <random>
#include <stdexcept>

#include "exercise.h"
namespace lesson_24 {

bool OptimizedLlr(std::span<const float> iq, float variance,
                  std::span<float> output) {
  // 中文：完整指令实现见kernel_solution.cc；此接口先经过能力检测。
  // English: Intrinsics live in kernel_solution.cc; dispatch checks capability.
  pusch::Require(iq.size() == output.size() && variance > 0,
                 "invalid spans/noise");
  if (!pusch::KernelAvailable("sve")) return false;
  LessonKernel(iq.data(), output.data(), iq.size(),
               2.f * std::sqrt(2.f) / variance);
  return true;
}
}  // namespace lesson_24
