#include "exercise.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <random>
#include <stdexcept>
namespace lesson_15 {
constexpr int kFftSize = 256;
int CpLength(int symbol) { return symbol == 0 ? 22 : 18; }
std::vector<Complex> OfdmModulate(const ResourceGrid& grid) {
  // TODO(1): 把中心化网格映射到FFT bins
  // English: Map centered tones to FFT bins
  // TODO(2): 调用酉IFFT
  // English: Apply unitary IFFT
  // TODO(3): 复制尾部形成CP
  // English: Prepend cyclic tail
  // TODO(4): 实现去CP和FFT逆过程
  // English: Remove CP and FFT
  pusch::Fail("NOT IMPLEMENTED: lesson 15");
}
}  // namespace lesson_15
