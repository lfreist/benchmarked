// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#include "benchmarked/benchmark.h"
#include "timed/Timer.h"

namespace benchmarked {

// ===== Benchmark =====================================================================================================
// ----- public --------------------------------------------------------------------------------------------------------
// _____________________________________________________________________________________________________________________
void Benchmark::Launch() {
  timed::WallTimer wall_timer;
  timed::CPUTimer cpu_timer;

  Fixture::SetUp();

  for (uint64_t iteration = 0; iteration < _iterations; ++iteration) {
    Fixture::Initialize();

    _cleanUp();

    wall_timer.start();
    cpu_timer.start();

    Run();

    cpu_timer.stop();
    wall_timer.stop();

    _results.emplace_back(cpu_timer.getTime(), wall_timer.getTime());

    Fixture::Reset();
  }

  Fixture::CleanUp();
  _launched = true;
}

}  // namespace benchmarked