// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#pragma once

#include <functional>

#include "benchmarked/launcher.h"
#include "benchmarked/reporter.h"
#include "benchmarked/benchmark.h"

#include "timed/Timer.h"

#ifndef BENCHMARKED_H_
#define BENCHMARKED_H_

namespace benchmarked::Internal {

class BenchmarkRegistrator {
 public:
  explicit BenchmarkRegistrator(const std::function<std::shared_ptr<BenchmarkBase>()> &builder) {
    LauncherConsole::GetInstance().RegisterBenchmarkBuilder(builder);
  }
};

}  // namespace benchmarked::Internal

#define BENCHMARK_UNIQUE_NAME_LINE2(name, line) name##line
#define BENCHMARK_UNIQUE_NAME_LINE(name, line) BENCHMARK_UNIQUE_NAME_LINE2(name, line)
#define BENCHMARK_UNIQUE_NAME(name) BENCHMARK_UNIQUE_NAME_LINE(name, __LINE__)

#define BENCHMARK_MAIN()\
int main(int argc, char** argv) {\
  benchmarked::LauncherConsole::GetInstance().Initialize(argc, argv);\
  benchmarked::LauncherConsole::GetInstance().Execute();\
  benchmarked::LauncherConsole::GetInstance().Report();\
  return 0;\
}


/**
 * Benchmark register macro
 * Example usage:
 * \code{.cpp}
 * // run `operation` 10 times and measure its performance
 * BENCHMARK("BenchmarkName", "BenchmarkType", "Description", 10) {
 *   operation();
 * }
 */
#define BENCHMARK(...)\
namespace benchmarked {\
class BENCHMARK_UNIQUE_NAME(__benchmark__) : public Benchmark {\
 public:\
  using Benchmark::Benchmark;\
 protected:\
  void Run() override;\
};\
Internal::BenchmarkRegistrator BENCHMARK_UNIQUE_NAME(benchmark_registrator)([]() { return std::make_shared<BENCHMARK_UNIQUE_NAME(__benchmark__)>(__VA_ARGS__);});\
}\
void benchmarked::BENCHMARK_UNIQUE_NAME(__benchmark__)::Run()


#define BENCHMARK_FIXTURE(fixture, ...)\
namespace benchmarked {\
class BENCHMARK_UNIQUE_NAME(__benchmark__) : public Benchmark, public fixture {\
 public:\
  using Benchmark::Benchmark;\
 protected:\
  void Run() override;\
};\
Internal::BenchmarkRegistrator BENCHMARK_UNIQUE_NAME(benchmark_registrator)([]() { return std::make_shared<BENCHMARK_UNIQUE_NAME(__benchmark__)>(__VA_ARGS__); });\
}\
void benchmarked::BENCHMARK_UNIQUE_NAME(__benchmark__)::Run()

#define BENCHMARK_CLASS(type, ...)\
namespace CppBenchmark { Internal::BenchmarkRegistrator BENCHMARK_UNIQUE_NAME(benchmark_registrator)([]() { return std::make_shared<type>(__VA_ARGS__); }); }

#define CPU_TIME_START(id)

#endif //BENCHMARKED_H_
