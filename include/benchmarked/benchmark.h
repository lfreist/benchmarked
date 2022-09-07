// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#pragma once

#include <thread>
#include <mutex>
#include <ctime>
#include <chrono>

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
                     std::function<void()> cleanUp = []() {}) : BenchmarkBase(name, type, description, iterations,
                                                                              std::move(cleanUp)) {}
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

/**
 * dynamic code benchmark base class
 * @tparam TimePoint
 */
template<typename TimePoint>
class CodeBenchmark {
  template<typename T>
  friend std::ostream &operator<<(std::ostream &os, const CodeBenchmark<T> &c_bm);
  friend class CodeBenchmarkHandler;
 public:
  CodeBenchmark() = default;
  CodeBenchmark(const CodeBenchmark &) = delete;
  CodeBenchmark(CodeBenchmark &&) = delete;
  CodeBenchmark &operator=(const CodeBenchmark &) = delete;
  CodeBenchmark &operator=(CodeBenchmark &&) = delete;

  virtual void start() = 0;
  virtual void stop() = 0;

  [[maybe_unused]] [[nodiscard]] virtual std::map<std::thread::id, double> getResults() const = 0;

 protected:
  std::mutex _pushToResultPairsMutex;
  std::map<std::thread::id, std::vector<std::pair<TimePoint, TimePoint>>> _resultPairs;
};

template<typename T>
std::ostream &operator<<(std::ostream &os, const CodeBenchmark<T> &c_bm) {
  os << c_bm._resultPairs.size() << " " << (c_bm._resultPairs.size() == 1 ? "thread" : "threads") << ":\n";
  double total = 0;
  for (const auto &[thread_id, time]: c_bm.getResults()) {
    total += time;
    if (c_bm._resultPairs.size() > 1) {
      os << "  " << time / 1000.0 / 1000.0 << " ms\n";
    }
  }
  if (c_bm._resultPairs.size() > 1) {
    os << "  --------\n";
  }
  os << "  " << total / 1000.0 / 1000.0 << " ms" << std::endl;
  return os;
}


class CodeBenchmarkThreadCPU : public CodeBenchmark<boost::chrono::thread_clock::time_point> {
 public:
  CodeBenchmarkThreadCPU() = default;
  CodeBenchmarkThreadCPU(const CodeBenchmarkThreadCPU &) = delete;
  CodeBenchmarkThreadCPU(CodeBenchmarkThreadCPU &&) = delete;
  CodeBenchmarkThreadCPU &operator=(const CodeBenchmarkThreadCPU &) = delete;
  CodeBenchmarkThreadCPU &operator=(CodeBenchmarkThreadCPU &&) = delete;

  void start() override;
  void stop() override;

  [[nodiscard]] std::map<std::thread::id, double> getResults() const override;
};


class CodeBenchmarkTotalCPU : public CodeBenchmark<std::clock_t> {
 public:
  CodeBenchmarkTotalCPU() = default;
  CodeBenchmarkTotalCPU(const CodeBenchmarkTotalCPU &) = delete;
  CodeBenchmarkTotalCPU(CodeBenchmarkTotalCPU &&) = delete;
  CodeBenchmarkTotalCPU &operator=(const CodeBenchmarkTotalCPU &) = delete;
  CodeBenchmarkTotalCPU &operator=(CodeBenchmarkTotalCPU &&) = delete;

  void start() override;
  void stop() override;

  [[nodiscard]] std::map<std::thread::id, double> getResults() const override;
};


class CodeBenchmarkWall : public CodeBenchmark<std::chrono::time_point<std::chrono::steady_clock>> {
 public:
  CodeBenchmarkWall() = default;
  CodeBenchmarkWall(const CodeBenchmarkWall &) = delete;
  CodeBenchmarkWall(CodeBenchmarkWall &&) = delete;
  CodeBenchmarkWall &operator=(const CodeBenchmarkWall &) = delete;
  CodeBenchmarkWall &operator=(CodeBenchmarkWall &&) = delete;

  void start() override;
  void stop() override;

  [[nodiscard]] std::map<std::thread::id, double> getResults() const override;
};


class CodeBenchmarkHandler {
  friend class CodeBenchmarkRegistrator;

 public:
  static CodeBenchmarkHandler &GetInstance();

  [[nodiscard]] std::string Report(const std::string &fmt = "console") const;

  void start(const std::string &name, uint8_t _bm_t_id);

  void stop(const std::string &name, uint8_t _bm_t_id);

 private:
  CodeBenchmarkHandler() = default;

  std::map<const std::string, CodeBenchmarkThreadCPU> _threadCPU_benchmarks;
  std::map<const std::string, CodeBenchmarkTotalCPU> _totalCPU_benchmarks;
  std::map<const std::string, CodeBenchmarkWall> _wall_benchmarks;
};

}  // namespace benchmarked
