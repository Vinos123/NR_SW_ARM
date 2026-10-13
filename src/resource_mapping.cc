#include "pusch/core.h"

namespace pusch {
ResourceGrid MapGrid(std::span<const Complex> symbols, const PuschConfig& c) {
  Require(static_cast<int>(symbols.size()) == DataReCount(c),
          "data RE mismatch");
  ResourceGrid grid(c.prbs);
  auto dmrs = Dmrs(c, 2);
  size_t data = 0;
  for (int l = 0; l < 14; ++l)
    for (int k = 0; k < grid.Subcarriers(); ++k)
      grid.At(l, k) = l == 2 && k % 2 == 0 ? dmrs[k / 2] : symbols[data++];
  return grid;
}

std::vector<Complex> ExtractData(const ResourceGrid& grid) {
  std::vector<Complex> data;
  for (int l = 0; l < 14; ++l)
    for (int k = 0; k < grid.Subcarriers(); ++k)
      if (!(l == 2 && k % 2 == 0)) data.push_back(grid.At(l, k));
  return data;
}
}  // namespace pusch
