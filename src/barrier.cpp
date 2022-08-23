// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#include <exception>

#include "benchmarked/barrier.h"

namespace benchmarked {

// ===== Barrier =======================================================================================================
// ----- public --------------------------------------------------------------------------------------------------------
// _____________________________________________________________________________________________________________________
Barrier::Barrier(unsigned threads) noexcept {
  if (threads == 0) { std::runtime_error("Barrier threads counter must not be zero (0)."); }
  _counter = threads;
  _threads = threads;
}

// _____________________________________________________________________________________________________________________
bool Barrier::Wait() noexcept {
  std::unique_lock<std::mutex> lock(_mutex);

  int generation = _generation;

  if (--_counter == 0) {
    _generation++;
    _counter = _threads;
    _condVar.notify_all();
    return true;
  }

  _condVar.wait(lock, [&, this]() { return generation != _generation; });

  return false;
}

}  // namespace benchmarked