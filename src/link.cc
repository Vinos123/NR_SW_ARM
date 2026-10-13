#include <algorithm>
#include <cmath>
#include <limits>
#include <numbers>

#include "pusch/core.h"

namespace pusch {
LinkResult RunLink(const PuschConfig& config, float snr_db, uint32_t seed,
                   bool practical_estimation, std::span<const Complex> taps) {
  Require(
      !std::isnan(snr_db) && snr_db != -std::numeric_limits<float>::infinity(),
      "invalid SNR");
  Require(taps.size() <= 18, "channel memory must fit the shortest CP");
  auto tb = MakeBits(TransportBlockSize(config), seed);
  auto plan = MakeCodingPlan(static_cast<int>(tb.size()), TargetRate(config));
  auto blocks = Segment(tb, plan);
  auto lengths =
      BlockLengths(DataReCount(config) * config.qm, plan.c, config.qm);
  Bits coded;
  for (int i = 0; i < plan.c; ++i) {
    auto full = LdpcEncode(blocks[i], plan);
    auto matched = RateMatch(full, plan, lengths[i], config.rv, config.qm);
    coded.insert(coded.end(), matched.begin(), matched.end());
  }
  uint32_t c_init = (uint32_t{config.rnti} << 15) + config.nid;
  auto symbols = Modulate(Scramble(coded, c_init), config.qm);
  auto waveform = OfdmModulate(MapGrid(symbols, config));
  if (!taps.empty()) waveform = ApplyChannel(waveform, taps);
  // 中文：SNR定义为单位功率RE的Es/N0；酉FFT保持复噪声方差。
  // English: SNR is Es/N0 for unit-energy REs; unitary FFT preserves variance.
  float variance = std::isinf(snr_db) ? 0.f : std::pow(10.f, -snr_db / 10.f);
  auto received =
      OfdmDemodulate(AddAwgn(waveform, variance, seed + 123), config.prbs);
  auto data = ExtractData(received);
  std::vector<Complex> h(received.Subcarriers(), {1.f, 0.f});
  // 中文：已知信道基线使用抽头的解析频响，实际估计模式只使用DM-RS。
  // English: Oracle mode uses analytic tap response; practical mode uses DM-RS.
  if (!taps.empty()) {
    for (int k = 0; k < received.Subcarriers(); ++k) {
      h[k] = 0;
      for (size_t delay = 0; delay < taps.size(); ++delay) {
        float phase = -2.f * std::numbers::pi_v<float> *
                      (k - received.Subcarriers() / 2) * delay / 256.f;
        h[k] += taps[delay] * std::polar(1.f, phase);
      }
    }
  }
  if (practical_estimation) h = EstimateChannel(received, config);
  std::vector<float> llrs;
  size_t index = 0;
  for (int l = 0; l < 14; ++l) {
    for (int k = 0; k < received.Subcarriers(); ++k) {
      if (l == 2 && k % 2 == 0) continue;
      // 中文：ZF后的噪声方差除以|H|²；深衰落输出擦除LLR。
      // English: ZF noise variance is N0/|H|²; deep fades yield erasure LLRs.
      float power = std::norm(h[k]);
      std::vector<float> soft(config.qm, 0.f);
      if (power > 1e-10f) {
        Complex equalized = data[index] / h[k];
        soft = Demodulate(std::span<const Complex>(&equalized, 1), config.qm,
                          std::max(variance / power, 1e-6f));
      }
      ++index;
      llrs.insert(llrs.end(), soft.begin(), soft.end());
    }
  }
  auto sequence = Gold(c_init, static_cast<int>(llrs.size()));
  for (size_t i = 0; i < llrs.size(); ++i)
    if (sequence[i]) llrs[i] = -llrs[i];
  Bits recovered_tb;
  LdpcDecoderAdapter decoder;
  size_t offset = 0;
  bool cb_ok = true;
  for (int i = 0; i < plan.c; ++i) {
    auto recovered =
        RateRecover(std::span<const float>(llrs).subspan(offset, lengths[i]),
                    plan, config.rv, config.qm);
    offset += lengths[i];
    auto decoded = decoder.Decode(recovered, plan);
    decoded.resize(plan.k_prime);
    if (plan.cb_crc) cb_ok &= CheckCrc(decoded, 24, 0x800063);
    decoded.resize(plan.k_prime - plan.cb_crc);
    recovered_tb.insert(recovered_tb.end(), decoded.begin(), decoded.end());
  }
  bool crc_ok = cb_ok && CheckCrc(recovered_tb, plan.a > 3824 ? 24 : 16,
                                  plan.a > 3824 ? 0x864cfb : 0x1021);
  int errors = 0;
  for (int i = 0; i < plan.a; ++i) errors += tb[i] != recovered_tb[i];
  return {crc_ok, errors, plan.a};
}
BlerResult MeasureBler(const PuschConfig& config, float snr_db, int blocks,
                       uint32_t seed) {
  Require(blocks > 0, "BLER needs positive block count");
  int errors = 0;
  for (int i = 0; i < blocks; ++i)
    errors += !RunLink(config, snr_db, seed + i).crc_ok;
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
}  // namespace pusch
