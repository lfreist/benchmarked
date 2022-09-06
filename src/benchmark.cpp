// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#include <iostream>
#include <strstream>

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

// _____________________________________________________________________________________________________________________
std::map<std::thread::id, double> CodeBenchmark::getTimePerThread() const {
  std::map<std::thread::id, double> result;
  for (const auto &[thread_id, intervals]: _resultPairs) {
    // MARK: look up is O(log n) -> this reference might be super unnecessary...
    auto& value = result[thread_id];
    value = 0;
    for (const auto &[start, end]: intervals) {
      value += double((end - start).count());
    }
  }
  return result;
}

// ----- << ------------------------------------------------------------------------------------------------------------
// _____________________________________________________________________________________________________________________
std::ostream &operator<<(std::ostream &os, const CodeBenchmark &c_bm) {
  os << c_bm._resultPairs.size() << " " << (c_bm._resultPairs.size() == 1 ? "thread" : "threads") << ":\n";
  double total = 0;
  for (const auto &[thread_id, time]: c_bm.getTimePerThread()) {
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

// ===== CodeBenchmarkHandler ==========================================================================================
// ----- public --------------------------------------------------------------------------------------------------------
// _____________________________________________________________________________________________________________________
std::string CodeBenchmarkHandler::Report(const std::string &fmt) const {
  const std::string sep(",");
  if (fmt == "csv") {
    std::stringstream ss;
    unsigned max_size = 0;
    for (const auto &[name, bm]: _benchmarks) {
      auto tmp_size = bm.getTimePerThread().size();
      max_size = tmp_size > max_size ? tmp_size : max_size;
    }
    ss << "name";
    for (unsigned i = 0; i < max_size; ++i) {
      ss << sep << i;
    }
    ss << '\n';
    for (const auto &[name, bm]: _benchmarks) {
      ss << name;
      unsigned nums = max_size;
      for (const auto &[thread_id, time]: bm.getTimePerThread()) {
        ss << sep << time;
        nums--;
      }
      for (;nums > 0; --nums) {
        ss << sep;
      }
      ss << '\n';
    }
    return ss.str();
  }
  else {
    std::strstream ss;
    for (const auto &[name, bm]: _benchmarks) {
      ss << name << " - " << bm << std::endl;
    }
    return ss.str();
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