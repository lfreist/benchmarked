// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#include <iostream>
#include <sstream>

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

// ===== CodeBenchmarkThreadCPU ========================================================================================
// ----- public --------------------------------------------------------------------------------------------------------
// _____________________________________________________________________________________________________________________
void CodeBenchmarkThreadCPU::start() {
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
void CodeBenchmarkThreadCPU::stop() {
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
std::map<std::thread::id, double> CodeBenchmarkThreadCPU::getResults() const {
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

// ===== CodeBenchmarkTotalCPU ========================================================================================
// ----- public --------------------------------------------------------------------------------------------------------
// _____________________________________________________________________________________________________________________
void CodeBenchmarkTotalCPU::start() {
  std::unique_lock locker(_pushToResultPairsMutex);
  auto &res_vector = _resultPairs[std::thread::id(0)];
  auto now = std::clock();
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
void CodeBenchmarkTotalCPU::stop() {
  std::unique_lock locker(_pushToResultPairsMutex);
  auto now = std::clock();
  auto &res_vector = _resultPairs[std::thread::id(0)];
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
std::map<std::thread::id, double> CodeBenchmarkTotalCPU::getResults() const {
  std::map<std::thread::id, double> result;
  // only one single element with thread::id == 0 is available here...
  for (const auto &[thread_id, intervals]: _resultPairs) {
    // MARK: look up is O(log n) -> this reference might be super unnecessary...
    auto& value = result[thread_id];
    value = 0;
    for (const auto &[start, end]: intervals) {
      value += double((end - start)) / CLOCKS_PER_SEC;
    }
    value *= 1000 * 1000 * 1000;
  }
  return result;
}

// ===== CodeBenchmarkThreadCPU ========================================================================================
// ----- public --------------------------------------------------------------------------------------------------------
// _____________________________________________________________________________________________________________________
void CodeBenchmarkWall::start() {
  std::unique_lock locker(_pushToResultPairsMutex);
  auto &res_vector = _resultPairs[std::thread::id(0)];
  auto now = std::chrono::steady_clock::now();
  if (res_vector.empty()) {
    res_vector.push_back({now, now});
    return;
  }
  if (res_vector.back().second == res_vector.back().first) {
    throw std::runtime_error(
        "Starting dynamic benchmark failed, since there already is a timer running for this benchmark id"
    );
  } else {
    res_vector.push_back({now, now});
  }
}

// _____________________________________________________________________________________________________________________
void CodeBenchmarkWall::stop() {
  std::unique_lock locker(_pushToResultPairsMutex);
  auto now = std::chrono::steady_clock::now();
  auto &res_vector = _resultPairs[std::thread::id(0)];
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
std::map<std::thread::id, double> CodeBenchmarkWall::getResults() const {
  std::map<std::thread::id, double> result;
  // only one single element with thread::id == 0 is in this map...
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

// ===== CodeBenchmarkHandler ==========================================================================================
// ----- public --------------------------------------------------------------------------------------------------------
// _____________________________________________________________________________________________________________________
std::string CodeBenchmarkHandler::Report(const std::string &fmt) const {
  const std::string sep(",");
  if (fmt == "csv") {
    std::stringstream ss;
    unsigned max_size = 0;
    for (const auto &[name, bm]: _threadCPU_benchmarks) {
      auto tmp_size = bm.getResults().size();
      max_size = tmp_size > max_size ? tmp_size : max_size;
    }
    for (const auto &[name, bm]: _totalCPU_benchmarks) {
      auto tmp_size = bm.getResults().size();
      max_size = tmp_size > max_size ? tmp_size : max_size;
    }
    for (const auto &[name, bm]: _wall_benchmarks) {
      auto tmp_size = bm.getResults().size();
      max_size = tmp_size > max_size ? tmp_size : max_size;
    }
    ss << "bm_type" << sep << "name";
    for (unsigned i = 0; i < max_size; ++i) {
      ss << sep << i;
    }
    ss << '\n';
    for (const auto &[name, bm]: _threadCPU_benchmarks) {
      ss << "thread CPU" << sep << name;
      unsigned nums = max_size;
      for (const auto &[thread_id, time]: bm.getResults()) {
        ss << sep << time;
        nums--;
      }
      for (;nums > 0; --nums) {
        ss << sep;
      }
      ss << '\n';
    }
    for (const auto &[name, bm]: _totalCPU_benchmarks) {
      ss << "total CPU" << sep << name;
      unsigned nums = max_size;
      for (const auto &[thread_id, time]: bm.getResults()) {
        ss << sep << time;
        nums--;
      }
      for (;nums > 0; --nums) {
        ss << sep;
      }
      ss << '\n';
    }
    for (const auto &[name, bm]: _wall_benchmarks) {
      ss << "Wall" << sep << name;
      unsigned nums = max_size;
      for (const auto &[thread_id, time]: bm.getResults()) {
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
    std::stringstream ss;
    ss << "Thread CPU:\n";
    for (const auto &[name, bm]: _threadCPU_benchmarks) {
      ss << name << " - " << bm << std::endl;
    }
    ss << "Total CPU:\n";
    for (const auto &[name, bm]: _totalCPU_benchmarks) {
      ss << name << " - " << bm << std::endl;
    }
    ss << "Wall:\n";
    for (const auto &[name, bm]: _wall_benchmarks) {
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
void CodeBenchmarkHandler::start(const std::string &name, uint8_t bm_t_id) {
  switch (bm_t_id) {
    case 0: _threadCPU_benchmarks[name].start(); break;
    case 1: _totalCPU_benchmarks[name].start(); break;
    case 2: _wall_benchmarks[name].start(); break;
    default: break;
  }
}

// _____________________________________________________________________________________________________________________
void CodeBenchmarkHandler::stop(const std::string &name, uint8_t bm_t_id) {
  switch (bm_t_id) {
    case 0: _threadCPU_benchmarks[name].stop(); break;
    case 1: _totalCPU_benchmarks[name].stop(); break;
    case 2: _wall_benchmarks[name].stop(); break;
    default: break;
  }
}

}  // namespace benchmarked