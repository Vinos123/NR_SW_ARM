#include <cstdio>
#include <cstdlib>

#include "pusch/core.h"

namespace pusch {
[[noreturn]] void Fail(const char* message) {
  std::fprintf(stderr, "%s\n", message);
  std::exit(1);
}

void Require(bool condition, const char* message) {
  if (!condition) Fail(message);
}
}  // namespace pusch
