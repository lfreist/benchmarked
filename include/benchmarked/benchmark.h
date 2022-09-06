// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#pragma once

#include <thread>
#include <mutex>

#include <boost/chrono.hpp>

#include "functional"

#include "benchmarked/fixture.h"
#include "benchmarked/benchmark_base.h"

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
  friend class CodeBenchmarkHandler;
 public:
  CodeBenchmark() = default;
  CodeBenchmark(const CodeBenchmark&) = delete;
  CodeBenchmark(CodeBenchmark&&) = delete;
  CodeBenchmark &operator=(const CodeBenchmark&) = delete;
  CodeBenchmark &operator=(CodeBenchmark&&) = delete;

  void start();
  void stop();

  [[nodiscard]] std::map<std::thread::id, double> getTimePerThread() const;

 protected:
  std::mutex _pushToResultPairsMutex;
  std::map<std::thread::id, std::vector<std::pair<boost::chrono::thread_clock::time_point, boost::chrono::thread_clock::time_point>>> _resultPairs;
};

std::ostream &operator<<(std::ostream &os, const CodeBenchmark &c_bm);

class CodeBenchmarkHandler {
  friend class CodeBenchmarkRegistrator;
 public:
  static CodeBenchmarkHandler& GetInstance();

  [[nodiscard]] std::string Report(const std::string &fmt = "console") const;

  void start(const std::string &name);
  void stop(const std::string &name);

 private:
  CodeBenchmarkHandler() = default;
  std::map<const std::string, CodeBenchmark> _benchmarks;
};

}  // namespace benchmarked
