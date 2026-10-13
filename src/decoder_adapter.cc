#include <algorithm>
#include <cmath>
#include <memory>

#include "ldpc_decoder_generic.h"
#include "pusch/core.h"
#include "srsran/srsvec/compare.h"

// 中文：用标量查找替代无关的SIMD比较依赖；不改变译码算法。
// English: Scalar find removes unrelated SIMD dependencies, not decoding logic.
namespace srsran::srsvec::detail {
const char* find(span<const char> x, const char* value) {
  return std::find(x.begin(), x.end(), *value);
}
}  // namespace srsran::srsvec::detail
namespace pusch {
Bits LdpcDecoderAdapter::Decode(std::span<const float> recovered,
                                const CodingPlan& plan, int iterations) const {
  Require(static_cast<int>(recovered.size()) == plan.n && iterations > 0,
          "invalid decoder input");
  std::vector<srsran::log_likelihood_ratio> quantized;
  for (int i = 0; i < plan.n; ++i) {
    // 中文：filler用+127表示确定的0，普通LLR缩放后饱和到[-63,63]。
    // English: Filler uses +127 (known zero); finite LLRs clip to [-63,63].
    bool filler = i + 2 * plan.z >= plan.k_prime && i + 2 * plan.z < plan.k;
    int value = filler ? 127
                       : static_cast<int>(std::round(
                             std::clamp(recovered[i] * 4.f, -63.f, 63.f)));
    quantized.emplace_back(value);
  }
  // 中文：大工作缓冲区放到堆上，避免Windows默认线程栈溢出。
  // English: Heap allocation avoids overflowing the default Windows stack.
  auto decoder = std::make_unique<srsran::ldpc_decoder_generic>(true);
  srsran::dynamic_bit_buffer output(plan.k);
  srsran::ldpc_decoder::configuration cfg;
  cfg.base_graph = plan.bg == 1 ? srsran::ldpc_base_graph_type::BG1
                                : srsran::ldpc_base_graph_type::BG2;
  cfg.lifting_size = static_cast<srsran::ldpc::lifting_size_t>(plan.z);
  cfg.nof_filler_bits = plan.filler;
  cfg.nof_crc_bits = plan.cb_crc ? 24 : plan.a > 3824 ? 24 : 16;
  cfg.max_iterations = iterations;
  // 中文：nullptr关闭CRC提前退出；返回nullopt不能直接判定失败。
  // English: nullptr disables CRC early stop; nullopt alone is not failure.
  decoder->decode(output, quantized, nullptr, cfg);
  Bits bits(plan.k);
  for (int i = 0; i < plan.k; ++i) bits[i] = output.extract<uint8_t>(i, 1);
  return bits;
}
}  // namespace pusch
