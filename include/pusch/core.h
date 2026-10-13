#ifndef PUSCH_CORE_H_
#define PUSCH_CORE_H_

#include <complex>
#include <cstdint>
#include <span>
#include <vector>

namespace pusch {
using Bits = std::vector<uint8_t>;
using Complex = std::complex<float>;

// 中文：教学配置只支持一个slot、单层、30 kHz、normal CP。
// English: Teaching profile: one slot, one layer, 30 kHz, normal CP.
struct PuschConfig {
  int prbs = 6;
  int qm = 2;
  int mcs = 4;
  int rv = 0;
  int slot = 0;
  uint16_t rnti = 1;
  uint16_t nid = 0;
};

// 中文：所有长度以比特为单位；z为提升尺寸，filler不属于有效载荷。
// English: Lengths are in bits; z is the lifting size, not payload length.
struct CodingPlan {
  int a = 0, b = 0, bg = 0, c = 0, cb_crc = 0;
  int k_prime = 0, kb = 0, z = 0, k = 0, n = 0, filler = 0;
};
struct CodeBlock {
  Bits bits;
  std::vector<bool> filler;
};

// 中文：按symbol-major存储，索引从0开始，每PRB包含12个子载波。
// English: Symbol-major storage, zero-based indexing, 12 tones per PRB.
class ResourceGrid {
 public:
  explicit ResourceGrid(int prbs);
  Complex& At(int symbol, int subcarrier);
  const Complex& At(int symbol, int subcarrier) const;
  int Subcarriers() const { return subcarriers_; }

 private:
  int subcarriers_;
  std::vector<Complex> values_;
};

// 中文：契约错误打印原因并以退出码1结束；比特必须是0或1。
// English: Contract failures print diagnostics and exit 1; bits are 0 or 1.
[[noreturn]] void Fail(const char* message);
void Require(bool condition, const char* message);
Bits MakeBits(int count, uint32_t seed = 1);
Bits Gold(uint32_t c_init, int count);
Bits Scramble(std::span<const uint8_t> bits, uint32_t c_init);
std::vector<Complex> Modulate(std::span<const uint8_t> bits, int qm);
// 中文：LLR=log(P(0)/P(1))；noise_variance是复噪声功率E[|n|²]。
// English: LLR=log(P(0)/P(1)); noise_variance is complex E[|n|²].
std::vector<float> Demodulate(std::span<const Complex> symbols, int qm,
                              float noise_variance);
std::vector<Complex> AddAwgn(std::span<const Complex> input, float variance,
                             uint32_t seed);
Bits HardDecide(std::span<const float> llrs);

// 中文：多项式省略最高项；MSB先行，初值0，不反射、不异或输出。
// English: Polynomial omits leading term; MSB first, init 0, no reflection.
uint32_t CalculateCrc(std::span<const uint8_t> bits, int width,
                      uint32_t polynomial);
Bits AttachCrc(std::span<const uint8_t> bits, int width, uint32_t polynomial);
bool CheckCrc(std::span<const uint8_t> bits, int width, uint32_t polynomial);
int SelectBaseGraph(int a, float rate);
CodingPlan MakeCodingPlan(int a, float rate);
std::vector<CodeBlock> Segment(std::span<const uint8_t> tb,
                               const CodingPlan& plan);
Bits LdpcEncode(const CodeBlock& block, const CodingPlan& plan);
bool CheckParity(std::span<const uint8_t> full_codeword,
                 const CodingPlan& plan);
Bits RateMatch(std::span<const uint8_t> full_codeword, const CodingPlan& plan,
               int e, int rv, int qm);
std::vector<float> RateRecover(std::span<const float> llrs,
                               const CodingPlan& plan, int rv, int qm);
std::vector<int> BlockLengths(int g, int blocks, int qm);

// 中文：srsRAN通用译码器仅经此接口调用；输出含filler，CRC由上层检查。
// English: Adapter isolates srsRAN; output includes filler; caller checks CRC.
class LdpcDecoderAdapter {
 public:
  Bits Decode(std::span<const float> recovered, const CodingPlan& plan,
              int iterations = 15) const;
};

int TransportBlockSize(const PuschConfig& config);
float TargetRate(const PuschConfig& config);
int DataReCount(const PuschConfig& config);
std::vector<Complex> Dmrs(const PuschConfig& config, int symbol);
ResourceGrid MapGrid(std::span<const Complex> symbols,
                     const PuschConfig& config);
// 中文：基线只支持完整14符号分配，DM-RS位于l=2，type1、port0、1个CDM组。
// English: Full 14-symbol allocation, DM-RS l=2, type1, port0, one CDM group.
std::vector<Complex> ExtractData(const ResourceGrid& grid);
std::vector<Complex> Transform(std::span<const Complex> input, bool inverse);
std::vector<Complex> OfdmModulate(const ResourceGrid& grid);
ResourceGrid OfdmDemodulate(std::span<const Complex> waveform, int prbs);
std::vector<Complex> ApplyChannel(std::span<const Complex> input,
                                  std::span<const Complex> taps);
std::vector<Complex> EstimateChannel(const ResourceGrid& grid,
                                     const PuschConfig& config);
std::vector<Complex> Equalize(std::span<const Complex> data,
                              std::span<const Complex> channel,
                              float noise_variance, bool mmse);
float PuschPowerDbm(float p_max, float p0, float alpha, float path_loss,
                    int prbs, int mu, float delta_tf, float adjustment);

struct LinkResult {
  bool crc_ok = false;
  int bit_errors = 0;
  int bits = 0;
};
LinkResult RunLink(const PuschConfig& config, float snr_db, uint32_t seed,
                   bool practical_estimation = false,
                   std::span<const Complex> taps = {});
struct BlerResult {
  int blocks, errors;
  double bler, low, high;
};
BlerResult MeasureBler(const PuschConfig& config, float snr_db, int blocks,
                       uint32_t seed);
}  // namespace pusch
#endif  // PUSCH_CORE_H_
