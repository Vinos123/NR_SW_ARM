#include "pusch/core.h"

namespace pusch {
uint32_t CalculateCrc(std::span<const uint8_t> bits, int width,
                      uint32_t polynomial) {
  Require(width == 16 || width == 24, "CRC width must be 16 or 24");
  uint32_t remainder = 0, mask = (1U << width) - 1;
  // 中文：反馈位是最高寄存器位与输入位的异或，输出为附加CRC。
  // English: Feedback XORs the top register bit with input; result is CRC.
  // Reference: TS 38.212 V17.6.0, 5.1.
  for (uint8_t bit : bits) {
    Require(bit <= 1, "CRC input is not binary");
    bool feedback = ((remainder >> (width - 1)) & 1U) ^ bit;
    remainder = (remainder << 1) & mask;
    if (feedback) remainder ^= polynomial;
  }
  return remainder;
}

Bits AttachCrc(std::span<const uint8_t> bits, int width, uint32_t polynomial) {
  uint32_t crc = CalculateCrc(bits, width, polynomial);
  Bits result(bits.begin(), bits.end());
  for (int i = width - 1; i >= 0; --i) result.push_back((crc >> i) & 1U);
  return result;
}

bool CheckCrc(std::span<const uint8_t> bits, int width, uint32_t polynomial) {
  return CalculateCrc(bits, width, polynomial) == 0;
}
}  // namespace pusch
