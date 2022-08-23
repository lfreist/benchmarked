// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#pragma once

#include "benchmarked/fixture.h"
#include "benchmarked/benchmark_base.h"

#ifndef BENCHMARKED_BENCHMARK_H_
#define BENCHMARKED_BENCHMARK_H_

namespace benchmarked {

class Benchmark : public BenchmarkBase, public virtual Fixture {
 public:
  explicit Benchmark(const std::string &name,
                     const std::string &type = "",
                     const std::string &description = "",
                     uint64_t iterations = 1) : BenchmarkBase(name, type, description, iterations) {}
  Benchmark(const Benchmark &) = delete;
  Benchmark(Benchmark &&) = delete;
  ~Benchmark() override = default;

  Benchmark &operator=(const Benchmark &) = delete;
  Benchmark &operator=(Benchmark &&) = delete;

 protected:
  virtual void Run() = 0;

 private:
  void Launch() override;
};

}  // namspace benchmarked

#endif //BENCHMARKED_BENCHMARK_H_
