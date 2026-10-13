#include "pusch/core.h"

namespace pusch {
ResourceGrid::ResourceGrid(int prbs) : subcarriers_(12 * prbs) {
  Require(prbs > 0 && prbs <= 275, "invalid PRB count");
  values_.resize(14 * subcarriers_);
}

Complex& ResourceGrid::At(int symbol, int subcarrier) {
  Require(symbol >= 0 && symbol < 14 && subcarrier >= 0 &&
              subcarrier < subcarriers_,
          "grid index out of range");
  return values_[symbol * subcarriers_ + subcarrier];
}

const Complex& ResourceGrid::At(int symbol, int subcarrier) const {
  Require(symbol >= 0 && symbol < 14 && subcarrier >= 0 &&
              subcarrier < subcarriers_,
          "grid index out of range");
  return values_[symbol * subcarriers_ + subcarrier];
}
}  // namespace pusch
