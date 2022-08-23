// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#include "benchmarked/reporter.h"

#include "timed/utils/Statistics.h"

#include "hwinfo/hwinfo.h"


namespace benchmarked {

// ===== ConsoleReporter ===============================================================================================
// ----- public --------------------------------------------------------------------------------------------------------
// _____________________________________________________________________________________________________________________
ConsoleReporter::ConsoleReporter(std::ostream &ostream) : _stream(ostream) {}

// _____________________________________________________________________________________________________________________
void ConsoleReporter::ReportInit(const std::string& launcherName) {
  hwinfo::CPU cpu;
  hwinfo::RAM ram;
  _stream << "\nBenchmark Report: " << launcherName << '\n'
          << "================================================================================\n"
          << "--- HARDWARE -------------------------------------------------------------------\n"
          << "CPU model:       " << cpu.modelName() << "\n"
          << "CPU cores:       " << cpu.numLogicalCores() << " (" << cpu.numCores() << ")\n"
          << "CPU clock speed: " << cpu.regularClockSpeedMHz() << " (" << cpu.maxClockSpeedMHz() << ") MHz\n"
          << "RAM size:        " << (static_cast<double>(ram.totalSizeBytes()) / 1000 / 1000 / 1000) << " GiB\n"
          << "================================================================================\n"
          << "--- BENCHMARKS -----------------------------------------------------------------\n"
          << std::flush;
}

// _____________________________________________________________________________________________________________________
void ConsoleReporter::ReportBenchmark(BenchmarkBase *benchmark) {
  if (benchmark->_results.empty()) {
    _stream << "No Results collected..." << std::endl;
    return;
  }
  std::vector<timed::Time> cpuTimes;
  std::vector<timed::Time> wallTimes;

  for (auto& res: benchmark->_results) {
    if (res.wallTime.getNanoseconds() != 0) { wallTimes.push_back(res.wallTime); }
    if (res.cpuTime.getNanoseconds() != 0) { cpuTimes.push_back(res.cpuTime); }
  }

  _stream << "--------------------------------------------------------------------------------\n"
          << "Benchmark:       " << benchmark->_name << "\n"
          << "Description:     " << benchmark->_description << "\n"
          << "Iterations:      " << benchmark->_iterations << "\n";
  if (benchmark->_iterations > 1) {
    if (!cpuTimes.empty()) {
      _stream << "  -------------------------------- CPU Time ------------------------------------\n"
              << "  min:           " << timed::utils::min(cpuTimes).getMilliseconds() << "ms\n"
              << "  max:           " << timed::utils::max(cpuTimes).getMilliseconds() << "ms\n"
              << "  mean:          " << timed::utils::mean(cpuTimes).getMilliseconds() << "ms\n"
              << "  median:        " << timed::utils::median(cpuTimes).getMilliseconds() << "ms\n"
              << "  % err          " << timed::utils::medianAbsolutePercentError(cpuTimes) << "\n";
    }
    if (!wallTimes.empty()) {
      _stream << "  ------------------------------- WALL Time ------------------------------------\n"
              << "  min:           " << timed::utils::min(wallTimes).getMilliseconds() << "ms\n"
              << "  max:           " << timed::utils::max(wallTimes).getMilliseconds() << "ms\n"
              << "  mean:          " << timed::utils::mean(wallTimes).getMilliseconds() << "ms\n"
              << "  median:        " << timed::utils::median(wallTimes).getMilliseconds() << "ms\n"
              << "  % err          " << timed::utils::medianAbsolutePercentError(wallTimes) << "\n";
    }
    _stream << std::flush;
  }
  else {
    if (!cpuTimes.empty()) {
      _stream << "  -------------------------------- CPU Time ------------------------------------\n"
              << "  cpu time:      " << cpuTimes[0].format("%ss%ms.%nsms") << "\n";
    }
    if (!wallTimes.empty()) {
      _stream << "  ------------------------------- WALL Time ------------------------------------\n"
              << "  wall time:     " << wallTimes[0].format("%ss%ms.%nsms") << "\n";
    }
    _stream << std::flush;
  }
}

// ===== CSVReporter ===================================================================================================
// ----- public --------------------------------------------------------------------------------------------------------
// _____________________________________________________________________________________________________________________
void CSVReporter::ReportInit(const std::string &launcherName) {
  _stream << "name" << _separator << "description" << _separator << "iterations" << _separator << "cpu-min"
          << _separator << "cpu-max" << _separator << "cpu-mean" << _separator << "cpu-median" << _separator
          << "cpu-%err" << _separator << "wall-min" << _separator << "wall-max" << _separator << "wall-mean"
          << _separator << "wall-median" << _separator << "wall-%err";
}

// _____________________________________________________________________________________________________________________
void CSVReporter::ReportBenchmark(BenchmarkBase *benchmark) {
  _stream << benchmark->_name << _separator << benchmark->_description << _separator << benchmark->_iterations << _separator << "cpu-min"
          << _separator << "cpu-max" << _separator << "cpu-mean" << _separator << "cpu-median" << _separator
          << "cpu-%err" << _separator << "wall-min" << _separator << "wall-max" << _separator << "wall-mean"
          << _separator << "wall-median" << _separator << "wall-%err";
  // TODO: finish
}

// ===== CompareReporter ===============================================================================================
// ----- public --------------------------------------------------------------------------------------------------------
// _____________________________________________________________________________________________________________________
void CompareReporter::Report(std::vector<std::shared_ptr<BenchmarkBase>> &benchmarks) {
  // TODO: implement
}

}  // namespace benchmarked