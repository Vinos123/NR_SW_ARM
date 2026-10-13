#include <algorithm>
#include <charconv>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <limits>
#include <string>

#include "pusch/core.h"
#include "pusch/simd.h"

namespace {
int ParseInt(const char* text) {
  int value = 0;
  auto result = std::from_chars(text, text + std::strlen(text), value);
  pusch::Require(result.ec == std::errc{} && *result.ptr == '\0',
                 "invalid integer");
  return value;
}
float ParseFloat(const char* text) {
  char* end = nullptr;
  float value = std::strtof(text, &end);
  pusch::Require(end != text && *end == '\0' && !std::isnan(value),
                 "invalid SNR");
  return value;
}
}  // namespace
int main(int argc, char** argv) {
  std::string command = argc > 1 ? argv[1] : "link";
  pusch::PuschConfig config;
  if (command == "link") {
    float snr =
        argc > 2 ? ParseFloat(argv[2]) : std::numeric_limits<float>::infinity();
    config.mcs = argc > 3 ? ParseInt(argv[3]) : 4;
    config.qm = config.mcs < 10 ? 2 : 4;
    auto result = pusch::RunLink(config, snr, 7);
    std::cout << "TBS=" << result.bits << " bit_errors=" << result.bit_errors
              << " CRC=" << (result.crc_ok ? "PASS" : "FAIL") << '\n';
    return result.crc_ok ? 0 : 1;
  }
  if (command == "bler") {
    int blocks = argc > 2 ? ParseInt(argv[2]) : 20;
    std::string path = argc > 3 ? argv[3] : "bler.csv";
    std::ofstream csv(path);
    pusch::Require(csv.good(), "cannot open CSV");
    csv << "snr_db,blocks,errors,bler,ci95_low,ci95_high,seed,mcs,tbs,"
           "throughput_bps\n";
    for (float snr : {-6.f, -3.f, 0.f, 3.f, 6.f}) {
      auto result = pusch::MeasureBler(config, snr, blocks, 1234);
      csv << snr << ',' << blocks << ',' << result.errors << ',' << result.bler
          << ',' << result.low << ',' << result.high << ",1234,4,"
          << pusch::TransportBlockSize(config) << ','
          << (1 - result.bler) * pusch::TransportBlockSize(config) / 0.0005
          << '\n';
      std::cout << "SNR=" << snr << " BLER=" << result.bler << '\n';
    }
    return 0;
  }
  if (command == "bench") {
    std::string kernel = argc > 2 ? argv[2] : "scalar";
    if (!pusch::KernelAvailable(kernel)) {
      std::cout << "SKIP: kernel not built or hardware unsupported\n";
      return 77;
    }
    std::vector<float> input(65539, 0.25f), output(input.size());
    // 中文：预热后测量7轮取中位数；checksum使输出可观察。
    // English: Warm up, take median of 7 rounds; checksum observes output.
    for (int i = 0; i < 100; ++i) pusch::QpskLlr(kernel, input, 0.5f, output);
    std::vector<double> times;
    for (int round = 0; round < 7; ++round) {
      auto start = std::chrono::steady_clock::now();
      for (int i = 0; i < 1000; ++i)
        pusch::QpskLlr(kernel, input, 0.5f, output);
      double ns = std::chrono::duration<double, std::nano>(
                      std::chrono::steady_clock::now() - start)
                      .count();
      times.push_back(ns / (1000 * input.size()));
    }
    std::sort(times.begin(), times.end());
    std::cout << "kernel=" << kernel << " compiler=" << __VERSION__
              << " ns/float=" << times[3] << " checksum=" << output.back()
              << '\n';
    return 0;
  }
  std::cerr << "Usage: pusch_cli link [snr_db|inf] [mcs] | bler [blocks] [csv] "
               "| bench [scalar|avx512|sve]\n";
  return 2;
}
