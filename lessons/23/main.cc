#include <cmath>
#include <iostream>
#include <limits>
#include <stdexcept>

#include "exercise.h"
int main() {
  // 中文：本课验收；契约失败结束程序，不把未完成练习算作通过。
  // English: Lesson acceptance; contract failures terminate, including
  // unfinished TODOs.
  if (!pusch::KernelAvailable("avx512")) {
    std::cout << "SKIP: avx512 not built or unsupported\n";
    return 77;
  }
  for (int n : {0, 1, 15, 16, 17, 33, 257}) {
    std::vector<float> iq(n, .25f), out(n), ref(n);
    pusch::QpskLlrScalar(iq, .5f, ref);
    pusch::Require(lesson_23::OptimizedLlr(iq, .5f, out), "dispatch");
    for (int j = 0; j < n; ++j)
      pusch::Require(std::abs(out[j] - ref[j]) < 1e-6f, "SIMD mismatch");
  }
  std::cout << "PASS lesson 23\n";
  return 0;
}
