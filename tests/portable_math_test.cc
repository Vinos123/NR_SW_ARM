#include <cmath>

// 中文：即使本机提供扩展宏，也移除它来复现Windows严格模式的条件。
// English: Remove any local extension macro to reproduce strict Windows mode.
#ifdef M_PI
#undef M_PI
#endif

#include "gtest/gtest.h"
#include "srsran/support/math/math_utils.h"

static_assert(srsran::TWOPI > 6.28f && srsran::TWOPI < 6.29f);

TEST(PortableMath, StandardPiWithoutExtensionMacro) {
  // 中文：依赖头文件必须在没有M_PI的情况下编译，并保持原来的精度。
  // English: The dependency must compile without M_PI and retain precision.
  EXPECT_FLOAT_EQ(srsran::TWOPI, 6.2831853071795864769f);
}
