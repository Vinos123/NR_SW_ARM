#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <random>
#include <stdexcept>

#include "exercise.h"
namespace lesson_14 {

ResourceGrid MapGrid(std::span<const Complex> symbols, const PuschConfig& c) {
  pusch::Require(static_cast<int>(symbols.size()) == pusch::DataReCount(c),
                 "data RE mismatch");
  ResourceGrid grid(c.prbs);
  auto dmrs = pusch::Dmrs(c, 2);
  size_t data = 0;
  for (int l = 0; l < 14; ++l)
    for (int k = 0; k < grid.Subcarriers(); ++k)
      grid.At(l, k) = l == 2 && k % 2 == 0 ? dmrs[k / 2] : symbols[data++];
  return grid;
}
}  // namespace lesson_14
