// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#include <ctime>
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
  if (res_vector.empty()) {
    res_vector.push_back({std::clock(), 0});
    return;
  }
  if (res_vector.back().second == 0) {
    throw std::runtime_error(
        "Starting dynamic benchmark failed, since there already is a timer running for this thread on this benchmark id"
    );
  } else {
    res_vector.push_back({std::clock(), 0});
  }
}

// _____________________________________________________________________________________________________________________
void CodeBenchmark::stop() {
  std::unique_lock locker(_pushToResultPairsMutex);
  auto &res_vector = _resultPairs[std::this_thread::get_id()];
  if (res_vector.empty()) { return; }
  auto &pair = res_vector.back();
  if (pair.second == 0) {
    pair.second = std::clock();
  } else {
    throw std::runtime_error(
        "Stopping code benchmark failed, since this benchmark id has not started a timer on this thread yet."
    );
  }
}

// ----- << ------------------------------------------------------------------------------------------------------------
// _____________________________________________________________________________________________________________________
std::ostream &operator<<(std::ostream &os, const CodeBenchmark &c_bm) {
  for (const auto &[key, value]: c_bm._resultPairs) {
    std::cout << "Thread: " << key << ":\n";
    double total;
    for (const auto &pair: value) {
      double time = 1000.0 * static_cast<double>(pair.second - pair.first) / CLOCKS_PER_SEC;
      total += time;
      std::cout << "  " << time * 1000.0 * 1000.0
                << " ns\n";
    }
    std::cout << " total: " << total * 1000 * 1000 << " ns";
  }
}

// ===== CodeBenchmarkHandler ==========================================================================================
// ----- public --------------------------------------------------------------------------------------------------------
// _____________________________________________________________________________________________________________________
CodeBenchmarkHandler::~CodeBenchmarkHandler() {
  for (const auto &[key, value]: _benchmarks) {
    std::cout << value << std::endl;
  }
}

// _____________________________________________________________________________________________________________________
CodeBenchmarkHandler &CodeBenchmarkHandler::GetInstance() {
  static CodeBenchmarkHandler instance;
  return instance;
}

// _____________________________________________________________________________________________________________________
void CodeBenchmarkHandler::createCodeBenchmark(unsigned int id) {
  _benchmarks[id];
}

// _____________________________________________________________________________________________________________________
void CodeBenchmarkHandler::start(unsigned int id) {
  _benchmarks[id].start();
}

// _____________________________________________________________________________________________________________________
void CodeBenchmarkHandler::stop(unsigned int id) {
  _benchmarks[id].stop();
}

}  // namespace benchmarked