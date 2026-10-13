#include "pusch/core.h"

#include <cmath>
#include <limits>

#include "gtest/gtest.h"
namespace pusch {
TEST(Modulation, RoundTrip) {
  for (int qm : {2, 4}) {
    auto bits = MakeBits(200, 3);
    EXPECT_EQ(bits, HardDecide(Demodulate(Modulate(bits, qm), qm, 0.1f)));
  }
}
TEST(Coding, CrcRoundTrip) {
  auto bits = MakeBits(100);
  EXPECT_TRUE(CheckCrc(AttachCrc(bits, 16, 0x1021), 16, 0x1021));
}
TEST(Coding, ParityAndDecode) {
  for (int a : {24, 292, 552, 3824, 4000, 9000}) {
    float rate = a == 9000 ? 0.7f : 0.5f;
    auto p = MakeCodingPlan(a, rate);
    auto blocks = Segment(MakeBits(a), p);
    for (auto& block : blocks) {
      auto full = LdpcEncode(block, p);
      EXPECT_TRUE(CheckParity(full, p)) << a;
      auto matched = RateMatch(full, p, p.n, 0, 2);
      std::vector<float> llrs;
      for (auto bit : matched) llrs.push_back(bit ? -10.f : 10.f);
      auto decoded = LdpcDecoderAdapter().Decode(RateRecover(llrs, p, 0, 2), p);
      EXPECT_EQ(decoded, block.bits) << a;
    }
  }
}
TEST(Waveform, RoundTrip) {
  PuschConfig c;
  auto data = Modulate(MakeBits(DataReCount(c) * 2), 2);
  auto grid = MapGrid(data, c);
  auto recovered = ExtractData(OfdmDemodulate(OfdmModulate(grid), c.prbs));
  ASSERT_EQ(data.size(), recovered.size());
  for (size_t i = 0; i < data.size(); ++i)
    EXPECT_LT(std::abs(data[i] - recovered[i]), 1e-5f);
}
TEST(Link, NoNoise) {
  for (int mcs : {4, 10, 16}) {
    PuschConfig c;
    c.mcs = mcs;
    c.qm = mcs < 10 ? 2 : 4;
    auto result = RunLink(c, std::numeric_limits<float>::infinity(), 7);
    EXPECT_TRUE(result.crc_ok) << mcs;
    EXPECT_EQ(result.bit_errors, 0) << mcs;
  }
}
}  // namespace pusch
