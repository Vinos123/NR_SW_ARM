#include <algorithm>
#include <cmath>
#include <memory>

#include "kiss_fft.h"
#include "pusch/core.h"

namespace pusch {
namespace {
void Validate(const PuschConfig& c) {
  Require(c.prbs > 0 && c.prbs <= 20 && c.slot >= 0 && c.slot < 20,
          "profile requires 1..20 PRBs and slot 0..19");
  Require(c.mcs >= 0 && c.mcs <= 16 && c.qm == (c.mcs < 10 ? 2 : 4),
          "MCS/Qm mismatch for table 5.1.3.1-1 subset");
  Require(c.nid <= 1007 && c.rv >= 0 && c.rv < 4, "invalid identity/RV");
}
constexpr int kFftSize = 256;
int CpLength(int symbol) { return symbol == 0 ? 22 : 18; }
}  // namespace
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
float TargetRate(const PuschConfig& c) {
  Validate(c);
  constexpr int kRates[] = {120, 157, 193, 251, 308, 379, 449, 526, 602,
                            679, 340, 378, 434, 490, 553, 616, 658};
  return kRates[c.mcs] / 1024.f;
}
int DataReCount(const PuschConfig& c) {
  Validate(c);
  // 中文：实际数据RE为14*12-6=162/PRB，不能替换TBS中的156上限。
  // English: Actual data REs are 162/PRB, unlike the TBS cap of 156.
  return c.prbs * 162;
}
int TransportBlockSize(const PuschConfig& c) {
  Validate(c);
  // 中文：38.214 6.1.4.2引用5.1.3.2；本配置无额外开销、单层。
  // English: 38.214 6.1.4.2 references 5.1.3.2; one layer, no overhead.
  float n_info = 156 * c.prbs * c.qm * TargetRate(c);
  constexpr int kSmallTbs[] = {
      24,   32,   40,   48,   56,   64,   72,   80,   88,   96,   104,  112,
      120,  128,  136,  144,  152,  160,  168,  176,  184,  192,  208,  224,
      240,  256,  272,  288,  304,  320,  336,  352,  368,  384,  408,  432,
      456,  480,  504,  528,  552,  576,  608,  640,  672,  704,  736,  768,
      808,  848,  888,  928,  984,  1032, 1064, 1128, 1160, 1192, 1224, 1256,
      1288, 1320, 1352, 1416, 1480, 1544, 1608, 1672, 1736, 1800, 1864, 1928,
      2024, 2088, 2152, 2216, 2280, 2408, 2472, 2536, 2600, 2664, 2728, 2792,
      2856, 2976, 3104, 3240, 3368, 3496, 3624, 3752, 3824};
  if (n_info <= 3824) {
    int n = std::max(3, static_cast<int>(std::floor(std::log2(n_info))) - 6);
    int quantized = std::max(24, (static_cast<int>(n_info) >> n) << n);
    return *std::lower_bound(std::begin(kSmallTbs), std::end(kSmallTbs),
                             quantized);
  }
  int n = static_cast<int>(std::floor(std::log2(n_info - 24))) - 5;
  int quantized = std::max(
      3840, static_cast<int>(std::round((n_info - 24) / (1 << n))) * (1 << n));
  int blocks = TargetRate(c) <= 0.25f ? (quantized + 24 + 3815) / 3816
               : quantized > 8424     ? (quantized + 24 + 8423) / 8424
                                      : 1;
  return 8 * blocks * ((quantized + 24 + 8 * blocks - 1) / (8 * blocks)) - 24;
}
std::vector<Complex> Dmrs(const PuschConfig& c, int symbol) {
  Validate(c);
  Require(symbol == 2, "profile DM-RS symbol must be 2");
  // 中文：nSCID=0、port0、参考PRB起点0；c_init使用64位避免乘法溢出。
  // English: nSCID=0, port0, reference PRB start 0; use 64-bit c_init math.
  // Reference: TS 38.211 V17.5.0, 6.4.1.1.1.
  uint64_t init =
      (uint64_t{1} << 17) * (14 * c.slot + symbol + 1) * (2 * c.nid + 1) +
      2 * c.nid;
  return Modulate(Gold(init & 0x7fffffffU, c.prbs * 12), 2);
}
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
std::vector<Complex> Transform(std::span<const Complex> input, bool inverse) {
  Require(!input.empty(), "empty FFT");
  auto* cfg =
      kiss_fft_alloc(static_cast<int>(input.size()), inverse, nullptr, nullptr);
  Require(cfg != nullptr, "FFT allocation failed");
  std::unique_ptr<void, decltype(&std::free)> owner(cfg, &std::free);
  std::vector<kiss_fft_cpx> in(input.size()), out(input.size());
  for (size_t i = 0; i < input.size(); ++i)
    in[i] = {input[i].real(), input[i].imag()};
  kiss_fft(cfg, in.data(), out.data());
  // 中文：采用酉归一化，两方向均除sqrt(N)，保持能量和噪声功率。
  // English: Unitary scaling divides both directions by sqrt(N).
  std::vector<Complex> result(input.size());
  float scale = 1.f / std::sqrt(static_cast<float>(input.size()));
  for (size_t i = 0; i < input.size(); ++i)
    result[i] = Complex(out[i].r, out[i].i) * scale;
  return result;
}
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
std::vector<Complex> ApplyChannel(std::span<const Complex> input,
                                  std::span<const Complex> taps) {
  Require(!taps.empty(), "empty channel");
  std::vector<Complex> output(input.size(), 0);
  for (size_t i = 0; i < input.size(); ++i)
    for (size_t j = 0; j < taps.size() && j <= i; ++j)
      output[i] += taps[j] * input[i - j];
  return output;
}
std::vector<Complex> EstimateChannel(const ResourceGrid& grid,
                                     const PuschConfig& c) {
  auto pilots = Dmrs(c, 2);
  Require(grid.Subcarriers() == c.prbs * 12, "pilot/grid mismatch");
  std::vector<Complex> channel(grid.Subcarriers());
  for (int k = 0; k < grid.Subcarriers(); k += 2)
    channel[k] = grid.At(2, k) / pilots[k / 2];
  // 中文：频域线性插值、末端保持、时间不变是假设，不是协议要求。
  // English: Linear frequency interpolation, edge hold and static time model.
  for (int k = 1; k < grid.Subcarriers(); k += 2)
    channel[k] = k + 1 < grid.Subcarriers()
                     ? (channel[k - 1] + channel[k + 1]) * 0.5f
                     : channel[k - 1];
  return channel;
}
std::vector<Complex> Equalize(std::span<const Complex> data,
                              std::span<const Complex> channel, float variance,
                              bool mmse) {
  Require(data.size() == channel.size() && variance >= 0,
          "equalizer size/noise");
  std::vector<Complex> output(data.size());
  for (size_t i = 0; i < data.size(); ++i) {
    float denominator = std::norm(channel[i]) + (mmse ? variance : 0.f);
    Require(denominator > 0, "zero channel: equalization undefined");
    output[i] = data[i] * std::conj(channel[i]) / denominator;
  }
  return output;
}
float PuschPowerDbm(float p_max, float p0, float alpha, float path_loss,
                    int prbs, int mu, float delta_tf, float adjustment) {
  Require(prbs > 0 && mu >= 0 && mu <= 4 && alpha >= 0 && alpha <= 1,
          "invalid power parameters");
  // 中文：单载波简化场景；没有实现多信道同时发射的功率优先级。
  // English: Single-carrier simplification; no simultaneous-channel priority.
  // Reference: TS 38.213 V17.6.0, 7.1.1.
  return std::min(p_max,
                  p0 + 10.f * std::log10(static_cast<float>(prbs * (1 << mu))) +
                      alpha * path_loss + delta_tf + adjustment);
}
}  // namespace pusch
