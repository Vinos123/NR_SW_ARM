#include <algorithm>
#include <cmath>
#include <fstream>
#include <limits>
#include <string>

#include "gtest/gtest.h"
#include "pusch/core.h"
#include "pusch/simd.h"
namespace pusch {
namespace {
std::vector<int> ReadInts(const std::string& file) {
  std::ifstream input(std::string(PUSCH_DATA_DIR) + "/" + file);
  Require(input.good(), "missing golden vector");
  std::vector<int> result;
  int value;
  while (input >> value) result.push_back(value);
  return result;
}
Bits ReadBits(const std::string& file) {
  auto values = ReadInts(file);
  return Bits(values.begin(), values.end());
}
}  // namespace
// 中文：这些文件由独立py3gpp实现产生，不由被测C++生成。
// English: Independent py3gpp generates these vectors, not the C++ under test.
TEST(Reference, GoldAndCrc) {
  EXPECT_EQ(Gold(12345, 256), ReadBits("gold.txt"));
  auto bits = ReadBits("crc_input.txt");
  EXPECT_EQ(AttachCrc(bits, 16, 0x1021), ReadBits("crc_16.txt"));
  EXPECT_EQ(AttachCrc(bits, 24, 0x864cfb), ReadBits("crc_24A.txt"));
  EXPECT_EQ(AttachCrc(bits, 24, 0x800063), ReadBits("crc_24B.txt"));
}
TEST(Reference, Constellations) {
  for (int qm : {2, 4}) {
    Bits labels;
    for (int word = 0; word < (1 << qm); ++word)
      for (int i = 0; i < qm; ++i) labels.push_back((word >> (qm - 1 - i)) & 1);
    auto symbols = Modulate(labels, qm);
    std::ifstream in(std::string(PUSCH_DATA_DIR) + "/qam" + std::to_string(qm) +
                     ".txt");
    ASSERT_TRUE(in.good());
    for (auto symbol : symbols) {
      float re, im;
      ASSERT_TRUE(static_cast<bool>(in >> re >> im));
      EXPECT_NEAR(symbol.real(), re, 1e-6f);
      EXPECT_NEAR(symbol.imag(), im, 1e-6f);
    }
  }
}
TEST(Reference, LdpcAllLiftingSetsAndRvs) {
  auto cases = ReadInts("ldpc_cases.txt");
  for (size_t j = 0; j < cases.size(); j += 4) {
    CodingPlan p;
    p.bg = cases[j];
    p.z = cases[j + 1];
    p.k_prime = cases[j + 2];
    p.k = (p.bg == 1 ? 22 : 10) * p.z;
    p.n = (p.bg == 1 ? 66 : 50) * p.z;
    p.filler = 7;
    std::string tag = "bg" + std::to_string(p.bg) + "_z" + std::to_string(p.z);
    SCOPED_TRACE(tag);
    auto bits = ReadBits(tag + "_input.txt");
    auto full = LdpcEncode({bits, std::vector<bool>(p.k, false)}, p);
    EXPECT_EQ(full, ReadBits(tag + "_full.txt"));
    EXPECT_TRUE(CheckParity(full, p));
    for (int rv = 0; rv < 4; ++rv)
      EXPECT_EQ(RateMatch(full, p, cases[j + 3], rv, 4),
                ReadBits(tag + "_rv" + std::to_string(rv) + ".txt"));
  }
}
TEST(Boundaries, GraphAndSegmentation) {
  EXPECT_EQ(SelectBaseGraph(292, .9f), 2);
  EXPECT_EQ(SelectBaseGraph(293, .9f), 1);
  EXPECT_EQ(SelectBaseGraph(3824, .67f), 2);
  EXPECT_EQ(SelectBaseGraph(3825, .67f), 1);
  EXPECT_EQ(SelectBaseGraph(4000, .25f), 2);
  EXPECT_EQ(SelectBaseGraph(4000, .2501f), 1);
  EXPECT_EQ(MakeCodingPlan(3824, .5f).c, 1);
  EXPECT_EQ(MakeCodingPlan(8424, .7f).c, 1);
  EXPECT_EQ(MakeCodingPlan(8426, .7f).c, 2);
  auto p = MakeCodingPlan(24, .5f);
  EXPECT_EQ(p.kb, 6);
  EXPECT_EQ(p.k, 10 * p.z);
  EXPECT_EXIT(MakeCodingPlan(0, .5f), ::testing::ExitedWithCode(1), "invalid");
  EXPECT_EXIT(SelectBaseGraph(24, 0), ::testing::ExitedWithCode(1), "invalid");
  EXPECT_EQ(BlockLengths(100, 3, 2), (std::vector<int>{32, 34, 34}));
}
TEST(Boundaries, CrcDetectsSingleBitErrors) {
  auto bits = AttachCrc(MakeBits(24), 16, 0x1021);
  for (size_t i = 0; i < bits.size(); ++i) {
    bits[i] ^= 1;
    EXPECT_FALSE(CheckCrc(bits, 16, 0x1021));
    bits[i] ^= 1;
  }
}
TEST(Receiver, RateRecoveryAndHarq) {
  auto p = MakeCodingPlan(552, .5f);
  auto block = Segment(MakeBits(552), p)[0];
  auto full = LdpcEncode(block, p);
  std::vector<float> combined(p.n, 0);
  for (int rv : {0, 2}) {
    auto bits = RateMatch(full, p, 1200, rv, 2);
    std::vector<float> llrs;
    for (auto bit : bits) llrs.push_back(bit ? -8.f : 8.f);
    auto recovered = RateRecover(llrs, p, rv, 2);
    for (int i = 0; i < p.n; ++i) combined[i] += recovered[i];
  }
  EXPECT_EQ(LdpcDecoderAdapter().Decode(combined, p), block.bits);
  auto repeated = RateRecover(std::vector<float>(4 * p.n, 1), p, 0, 2);
  EXPECT_GT(*std::min_element(repeated.begin(), repeated.end()), 1);
}
TEST(Waveform, PilotAndFlatChannel) {
  PuschConfig c;
  auto data = Modulate(MakeBits(DataReCount(c) * 2), 2);
  auto grid = MapGrid(data, c);
  Complex gain(0.8f, 0.2f);
  for (int l = 0; l < 14; ++l)
    for (int k = 0; k < grid.Subcarriers(); ++k) grid.At(l, k) *= gain;
  auto h = EstimateChannel(grid, c);
  for (auto value : h) EXPECT_LT(std::abs(value - gain), 1e-6f);
  EXPECT_EQ(OfdmModulate(grid).size(), 3840U);
  EXPECT_EXIT(grid.At(14, 0), ::testing::ExitedWithCode(1), "range");
  EXPECT_EXIT(grid.At(0, -1), ::testing::ExitedWithCode(1), "range");
}
TEST(Receiver, NoiseAndEqualizer) {
  auto noisy = AddAwgn(std::vector<Complex>(100000, 0), .5f, 1);
  double power = 0;
  for (auto x : noisy) power += std::norm(x);
  EXPECT_NEAR(power / noisy.size(), .5, .01);
  std::vector<Complex> x{{1, 1}}, h{{.5f, .5f}};
  std::vector<Complex> y{x[0] * h[0]};
  EXPECT_LT(std::abs(Equalize(y, h, .1f, false)[0] - x[0]), 1e-6f);
  auto biased = Equalize(y, h, .1f, true)[0];
  EXPECT_LT(
      std::abs(biased / (std::norm(h[0]) / (std::norm(h[0]) + .1f)) - x[0]),
      1e-6f);
}
TEST(Link, ConfigurationsAndStatistics) {
  PuschConfig initial;
  EXPECT_TRUE(
      RunLink(initial, std::numeric_limits<float>::infinity(), 4, true).crc_ok);
  // 中文：每个RV使用足够E覆盖母码；有限E的单独RV不保证可译码。
  // English: Cover the mother code for each RV; a short single RV may fail.
  auto plan = MakeCodingPlan(576, .5f);
  auto block = Segment(MakeBits(576), plan)[0];
  auto full = LdpcEncode(block, plan);
  for (int rv = 0; rv < 4; ++rv) {
    auto sent = RateMatch(full, plan, 2 * plan.n, rv, 2);
    std::vector<float> soft;
    for (auto bit : sent) soft.push_back(bit ? -10.f : 10.f);
    EXPECT_EQ(LdpcDecoderAdapter().Decode(RateRecover(soft, plan, rv, 2), plan),
              block.bits);
  }
  PuschConfig c;
  c.prbs = 20;
  c.qm = 4;
  c.mcs = 16;
  EXPECT_TRUE(RunLink(c, std::numeric_limits<float>::infinity(), 5).crc_ok);
  auto result = MeasureBler({}, std::numeric_limits<float>::infinity(), 3, 1);
  EXPECT_EQ(result.errors, 0);
  EXPECT_GT(result.high, 0);
  EXPECT_GE(result.low, 0);
  EXPECT_FLOAT_EQ(PuschPowerDbm(23, -80, 1, 120, 6, 1, 0, 0), 23);
}
TEST(Simd, ScalarMatchesDemodulator) {
  auto symbols = Modulate(MakeBits(258), 2);
  std::vector<float> iq;
  for (auto z : symbols) {
    iq.push_back(z.real());
    iq.push_back(z.imag());
  }
  std::vector<float> result(iq.size());
  QpskLlrScalar(iq, .3f, result);
  auto reference = Demodulate(symbols, 2, .3f);
  for (size_t i = 0; i < result.size(); ++i)
    EXPECT_NEAR(result[i], reference[i], 1e-5f);
}
void TestKernel(const char* kernel) {
  if (!KernelAvailable(kernel))
    GTEST_SKIP() << "Not built or hardware unavailable";
  // 中文：偏移地址与短尾部检查掩码是否真正防止越界写。
  // English: Offset pointers and short tails check masked writes.
  for (int count : {0, 1, 2, 15, 16, 17, 31, 33, 257}) {
    std::vector<float> input(count + 2, .25f), output(count + 2, -999.f),
        expected(count);
    QpskLlrScalar(std::span(input).subspan(1, count), .3f, expected);
    EXPECT_TRUE(QpskLlr(kernel, std::span(input).subspan(1, count), .3f,
                        std::span(output).subspan(1, count)));
    EXPECT_EQ(output.front(), -999.f);
    EXPECT_EQ(output.back(), -999.f);
    for (int i = 0; i < count; ++i)
      EXPECT_NEAR(output[i + 1], expected[i], 1e-6f);
  }
}
TEST(Simd, Avx512) { TestKernel("avx512"); }
TEST(Simd, Sve) { TestKernel("sve"); }
}  // namespace pusch

namespace pusch {
// 中文：独立分段向量用-1表示filler，与本工程的独立标记逐位置对比。
// English: Independent segmentation uses -1 for filler; compare each position.
TEST(Reference, SegmentationAndTbs) {
  for (int a : {24, 576, 3824, 4000, 9000}) {
    std::string tag = "tb" + std::to_string(a);
    auto tb = ReadBits(tag + "_input.txt");
    auto plan = MakeCodingPlan(a, a == 9000 ? .7f : .5f);
    auto blocks = Segment(tb, plan);
    auto expected = ReadInts(tag + "_blocks.txt");
    ASSERT_EQ(expected.size(), static_cast<size_t>(plan.c * plan.k));
    for (int c = 0; c < plan.c; ++c) {
      for (int j = 0; j < plan.k; ++j) {
        int value = expected[c * plan.k + j];
        EXPECT_EQ(blocks[c].filler[j], value == -1);
        EXPECT_EQ(blocks[c].bits[j], value == -1 ? 0 : value);
      }
    }
  }
  auto cases = ReadInts("tbs_cases.txt");
  for (size_t i = 0; i < cases.size(); i += 3) {
    PuschConfig c;
    c.prbs = cases[i];
    c.mcs = cases[i + 1];
    c.qm = c.mcs < 10 ? 2 : 4;
    EXPECT_EQ(TransportBlockSize(c), cases[i + 2]) << c.prbs << " " << c.mcs;
  }
}
}  // namespace pusch

namespace pusch {
TEST(Link, MultipathOracleAndDmrs) {
  std::vector<Complex> taps{{.9f, .1f}, {.2f, -.1f}};
  for (bool practical : {false, true}) {
    auto result =
        RunLink({}, std::numeric_limits<float>::infinity(), 9, practical, taps);
    EXPECT_TRUE(result.crc_ok);
    EXPECT_EQ(result.bit_errors, 0);
  }
}
}  // namespace pusch
