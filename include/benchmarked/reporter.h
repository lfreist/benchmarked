// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#pragma once

#include <iostream>

#include "benchmarked/benchmark_base.h"

#ifndef BENCHMARKED_REPORTER_H_
#define BENCHMARKED_REPORTER_H_

namespace benchmarked {

class Reporter {
 public:
  Reporter() = default;
  virtual ~Reporter() = default;

  virtual void ReportInit(const std::string& launcherName) {};
  virtual void ReportBenchmark(BenchmarkBase *benchmark) {};
};

class ConsoleReporter : public Reporter {
 public:
  ConsoleReporter() : _stream(std::cout) {}
  explicit ConsoleReporter(std::ostream& ostream);

  void ReportInit(const std::string& launcherName) override;
  void ReportBenchmark(BenchmarkBase *benchmark) override;

 private:
  std::ostream& _stream;
};

class CSVReporter : public Reporter {
 public:
  explicit CSVReporter(std::ostream& stream, const std::string& separator = ",") : _stream(stream), _separator(separator) {}

  void ReportInit(const std::string& launcherName) override;
  void ReportBenchmark(BenchmarkBase *benchmark) override;

 private:
  std::ostream& _stream;
  std::string _separator;
};

class CompareReporter {
 public:
  explicit CompareReporter(std::ostream& stream) : _stream(stream) {}

  void Report(std::vector<std::shared_ptr<BenchmarkBase>>& benchmarks);

 private:
  std::ostream& _stream;
};

}  // namespace benchmarked

#endif //BENCHMARKED_REPORTER_H_
