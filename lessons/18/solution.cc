#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <random>
#include <stdexcept>

#include "exercise.h"
namespace lesson_18 {

BlerResult MeasureBler(const PuschConfig& config, float snr_db, int blocks,
                       uint32_t seed) {
  pusch::Require(blocks > 0, "BLER needs positive block count");
  int errors = 0;
  for (int i = 0; i < blocks; ++i)
    errors += !pusch::RunLink(config, snr_db, seed + i).crc_ok;
  // 中文：Wilson 95%区间；零错误不代表真实BLER为零。
  // English: Wilson 95% interval; zero observed errors is not zero true BLER.
  double p = static_cast<double>(errors) / blocks, z = 1.95996398454;
  double den = 1 + z * z / blocks;
  double center = (p + z * z / (2 * blocks)) / den;
  double half =
      z * std::sqrt(p * (1 - p) / blocks + z * z / (4. * blocks * blocks)) /
      den;
  return {blocks, errors, p, std::max(0., center - half),
          std::min(1., center + half)};
}
}  // namespace lesson_18
