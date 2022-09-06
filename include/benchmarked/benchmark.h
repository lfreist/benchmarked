// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#pragma once

#include <thread>
#include <mutex>

#include "functional"

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
                     uint64_t iterations = 1,
                     std::function<void()> cleanUp = [](){}) : BenchmarkBase(name, type, description, iterations, std::move(cleanUp)) {}
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

class CodeBenchmark {
  friend std::ostream &operator<<(std::ostream &os, const CodeBenchmark &c_bm);
 public:
  CodeBenchmark() = default;
  CodeBenchmark(const CodeBenchmark&) = delete;
  CodeBenchmark(CodeBenchmark&&) = delete;
  CodeBenchmark &operator=(const CodeBenchmark&) = delete;
  CodeBenchmark &operator=(CodeBenchmark&&) = delete;

  void start();
  void stop();

 protected:
  std::mutex _pushToResultPairsMutex;
  std::map<std::thread::id, std::vector<std::pair<std::clock_t, std::clock_t>>> _resultPairs;
};

std::ostream &operator<<(std::ostream &os, const CodeBenchmark &c_bm);

class CodeBenchmarkHandler {
  friend class CodeBenchmarkRegistrator;
 public:
  void Report();
  static CodeBenchmarkHandler& GetInstance();

  void start(unsigned id);
  void stop(unsigned id);

 private:
  CodeBenchmarkHandler() = default;
  std::map<unsigned, CodeBenchmark> _benchmarks;
};

}  // namspace benchmarked

#endif //BENCHMARKED_BENCHMARK_H_
