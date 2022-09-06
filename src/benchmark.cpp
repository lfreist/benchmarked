// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#include <iostream>

#include "benchmarked/benchmark.h"
#include "timed/Timer.h"

namespace benchmarked {

// ===== Benchmark =====================================================================================================
// ----- public --------------------------------------------------------------------------------------------------------
// _____________________________________________________________________________________________________________________
void Benchmark::Launch() {
  timed::WallTimer wall_timer;
  timed::CPUTimer cpu_timer;

  SetUp();

  for (uint64_t iteration = 0; iteration < _iterations; ++iteration) {
    Initialize();

    _cleanUp();

    wall_timer.start();
    cpu_timer.start();

    Run();

    cpu_timer.stop();
    wall_timer.stop();

    _results.emplace_back(cpu_timer.getTime(), wall_timer.getTime());

    Reset();
  }

  CleanUp();
  _launched = true;
}

// ===== CodeBenchmark =================================================================================================
// ----- public --------------------------------------------------------------------------------------------------------
// _____________________________________________________________________________________________________________________
void CodeBenchmark::start() {
  std::unique_lock locker(_pushToResultPairsMutex);
  auto &res_vector = _resultPairs[std::this_thread::get_id()];
  auto now = boost::chrono::thread_clock::now();
  if (res_vector.empty()) {
    res_vector.push_back({now, now});
    return;
  }
  if (res_vector.back().second == res_vector.back().first) {
    throw std::runtime_error(
      "Starting dynamic benchmark failed, since there already is a timer running for this thread on this benchmark id"
    );
  } else {
    res_vector.push_back({now, now});
  }
}

// _____________________________________________________________________________________________________________________
void CodeBenchmark::stop() {
  std::unique_lock locker(_pushToResultPairsMutex);
  auto now = boost::chrono::thread_clock::now();
  auto &res_vector = _resultPairs[std::this_thread::get_id()];
  if (res_vector.empty()) { return; }
  auto &pair = res_vector.back();
  if (pair.second == pair.first) {
    pair.second = now;
  }
  else {
    throw std::runtime_error(
      "Stopping code benchmark failed, since this benchmark id has not started a timer on this thread yet."
    );
  }
}

// ----- << ------------------------------------------------------------------------------------------------------------
// _____________________________________________________________________________________________________________________
std::ostream &operator<<(std::ostream &os, const CodeBenchmark &c_bm) {
  std::cout << c_bm._resultPairs.size() << " " << (c_bm._resultPairs.size() == 1 ? "thread" : "threads") << ":\n";
  for (const auto &[key, value]: c_bm._resultPairs) {
    double total;
    for (const auto &pair: value) {
      total += double((pair.second - pair.first).count());
    }
    std::cout << total / 1000.0 / 1000.0 << " ms\n";
  }
}

// ===== CodeBenchmarkHandler ==========================================================================================
// ----- public --------------------------------------------------------------------------------------------------------
// _____________________________________________________________________________________________________________________
void CodeBenchmarkHandler::Report() {
  for (const auto &[key, value]: _benchmarks) {
    std::cout << key << " - " << value << std::endl;
  }
}

// _____________________________________________________________________________________________________________________
CodeBenchmarkHandler &CodeBenchmarkHandler::GetInstance() {
  static CodeBenchmarkHandler instance;
  return instance;
}

// _____________________________________________________________________________________________________________________
void CodeBenchmarkHandler::start(const std::string &name) {
  _benchmarks[name].start();
}

// _____________________________________________________________________________________________________________________
void CodeBenchmarkHandler::stop(const std::string &name) {
  _benchmarks[name].stop();
}

}  // namespace benchmarked