#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <random>
#include <stdexcept>

#include "exercise.h"
namespace lesson_01 {

int CountResourceElements(int prbs) {
  pusch::Require(prbs > 0, "positive PRBs required");
  // 中文：normal CP的slot有14个符号，每个PRB有12个子载波。
  // English: A normal-CP slot has 14 symbols; a PRB has 12 subcarriers.
  return prbs * 12 * 14;
}
}  // namespace lesson_01
