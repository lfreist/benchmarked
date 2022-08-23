// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#pragma once

#include <condition_variable>
#include <mutex>

#ifndef BENCHMARKER_BARRIER_H_
#define BENCHMARKER_BARRIER_H_

namespace benchmarked {

/**
 * Any thread registered for this barrier must stop here until all other threads have reached this barrier
 *
 * Thread-safe.
 */
class Barrier {
 public:
  /// \param threads: count of threads to wait at this barrier
  explicit Barrier(unsigned threads) noexcept;
  Barrier(const Barrier&) = delete;
  Barrier(Barrier&&) = delete;

  Barrier& operator=(const Barrier&) = delete;
  Barrier& operator=(Barrier&&) = delete;

  bool Wait() noexcept;

 private:
  std::mutex _mutex;
  std::condition_variable _condVar;
  int _counter = 0;
  int _generation = 0;
  unsigned _threads = 0;
};

}  // benchmarked

#endif //BENCHMARKER_BARRIER_H_
