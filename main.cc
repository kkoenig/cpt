#define ANKERL_NANOBENCH_IMPLEMENT
#include "cpt.h"
#include <nanobench.h>

int main() {
  const char *input_string = "1234586";
  ankerl::nanobench::doNotOptimizeAway(input_string);

  double d = 1.0;
  ankerl::nanobench::Bench bench;
  bench.warmup(100000);
  bench.run("atoi", [&] {
    const auto result = atoi(input_string);
    ankerl::nanobench::doNotOptimizeAway(result);
  });
  bench.run("sscanf", [&] {
    uint32_t result;
    (void)sscanf(input_string, "%u", &result);
    ankerl::nanobench::doNotOptimizeAway(result);
  });
  bench.run("strtoul", [&] {
    const uint32_t result = strtoul(input_string, NULL, 10);
    ankerl::nanobench::doNotOptimizeAway(result);
  });
  bench.run("read_u32", [&] {
    const uint32_t result = read_u32(input_string);
    ankerl::nanobench::doNotOptimizeAway(result);
  });
  bench.run("null", [&] { ankerl::nanobench::doNotOptimizeAway(0); });
}