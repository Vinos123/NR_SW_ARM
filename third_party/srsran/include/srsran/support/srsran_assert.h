#pragma once
#include <cstdlib>
#include <cstdio>
// Portability shim: always-active assertions; logging intentionally omitted.
#define srsran_assert(c, ...) do { if (!(c)) { std::fprintf(stderr, "srsRAN assertion: %s (%s:%d)\n", #c, __FILE__, __LINE__); std::abort(); } } while (false)
#define srsran_sanity_check(c, ...) srsran_assert(c, __VA_ARGS__)
#define srsran_assertion_failure(...) srsran_assert(false, __VA_ARGS__)
#define srsran_assume(c) srsran_assert(c, "assumption failed")
#define report_fatal_error_if_not(c, ...) srsran_assert(c, __VA_ARGS__)
