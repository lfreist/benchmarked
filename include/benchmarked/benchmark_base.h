// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#pragma once

#include <string>
#include <vector>
#include <memory>

#include "timed/TimeUtils.h"

#ifndef BENCHMARKED_BENCHMARK_BASE_H_
#define BENCHMARKED_BENCHMARK_BASE_H_

namespace benchmarked {

struct Result {
  Result(timed::Time cpu, timed::Time wall) {
    cpuTime = cpu;
    wallTime = wall;
  }

  timed::Time cpuTime;
  timed::Time wallTime;
};

class BenchmarkBase {
  friend class Launcher;
  friend class LauncherConsole;
  friend class ConsoleReporter;
  friend class CSVReporter;
  friend class JSONReporter;
  friend class CompareReporter;
 public:
  explicit BenchmarkBase(const std::string &name, const std::string &type, const std::string &description, uint64_t iterations)
    : _name(name), _type(type), _description(description), _iterations(iterations) {}
  virtual ~BenchmarkBase() = default;

  virtual void Launch() = 0;

 protected:
  bool _launched = false;
  uint64_t _iterations = 0;
  std::string _name;
  std::string _type;
  std::string _description;
  std::vector<Result> _results;
};

}  // namespace benchmarked


#endif //BENCHMARKED_BENCHMARK_BASE_H_
