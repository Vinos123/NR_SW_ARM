#include "pusch/core.h"

namespace pusch {
namespace {
constexpr int kFftSize = 256;
int CpLength(int symbol) { return symbol == 0 ? 22 : 18; }
}  // namespace

std::vector<Complex> OfdmModulate(const ResourceGrid& grid) {
  Require(grid.Subcarriers() <= 240, "FFT profile allows at most 20 PRBs");
  std::vector<Complex> waveform;
  for (int l = 0; l < 14; ++l) {
    std::vector<Complex> bins(kFftSize, 0);
    for (int k = 0; k < grid.Subcarriers(); ++k)
      bins[(k - grid.Subcarriers() / 2 + kFftSize) % kFftSize] = grid.At(l, k);
    auto time = Transform(bins, true);
    // 中文：mu=1、Nfft=256时，每slot首符号CP=22，其余=18。
    // English: At mu=1, Nfft=256, CP is 22 for symbol 0 and 18 otherwise.
    // Reference: TS 38.211 V17.5.0, 5.3.1; baseband fc=0 convention.
    waveform.insert(waveform.end(), time.end() - CpLength(l), time.end());
    waveform.insert(waveform.end(), time.begin(), time.end());
  }
  return waveform;
}

ResourceGrid OfdmDemodulate(std::span<const Complex> waveform, int prbs) {
  Require(waveform.size() == 3840 && prbs > 0 && prbs <= 20,
          "expected 3840 samples at 7.68 Msps");
  ResourceGrid grid(prbs);
  size_t offset = 0;
  for (int l = 0; l < 14; ++l) {
    offset += CpLength(l);
    auto bins = Transform(waveform.subspan(offset, kFftSize), false);
    offset += kFftSize;
    for (int k = 0; k < grid.Subcarriers(); ++k)
      grid.At(l, k) = bins[(k - grid.Subcarriers() / 2 + kFftSize) % kFftSize];
  }
  return grid;
}
}  // namespace pusch
