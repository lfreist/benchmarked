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
void ConsoleReporter::ReportInit(const std::string &launcherName) {
  hwinfo::CPU cpu;
  hwinfo::RAM ram;
  _stream << "\nBenchmark Report: " << launcherName << '\n'
          << "================================================================================\n"
          << "--- HARDWARE -------------------------------------------------------------------\n"
          << "CPU model:       " << cpu.modelName() << "\n"
          << "CPU cores:       " << cpu.numLogicalCores() << " (" << cpu.numPhysicalCores() << ")\n"
          << "CPU clock speed: " << cpu.regularClockSpeed_kHz() << " (" << cpu.maxClockSpeed_kHz() << ") MHz\n"
          << "RAM size:        " << (static_cast<double>(ram.totalSize_Bytes()) / 1000 / 1000 / 1000) << " GiB\n"
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

  std::vector<double> cpuTimes_ms;
  std::vector<double> wallTimes_ms;

  for (auto &res: benchmark->_results) {
    if (res.wallTime.getNanoseconds() != 0) { wallTimes_ms.push_back(res.wallTime.getMilliseconds()); }
    if (res.cpuTime.getNanoseconds() != 0) { cpuTimes_ms.push_back(res.cpuTime.getMilliseconds()); }
  }

  _stream << "--------------------------------------------------------------------------------\n"
          << "Benchmark:       " << benchmark->_name << "\n"
          << "Description:     " << benchmark->_description << "\n"
          << "Iterations:      " << benchmark->_iterations << "\n";
  if (benchmark->_iterations > 1) {
    if (!cpuTimes_ms.empty()) {
      _stream << "  -------------------------------- CPU Time ------------------------------------\n"
              << "  min:           " << timed::utils::min(cpuTimes_ms) << " ms\n"
              << "  max:           " << timed::utils::max(cpuTimes_ms) << " ms\n"
              << "  mean:          " << timed::utils::mean(cpuTimes_ms) << " ms\n"
              << "  median:        " << timed::utils::median(cpuTimes_ms) << " ms\n"
              << "  % err          " << timed::utils::medianAbsolutePercentError(cpuTimes_ms) << "\n";
    }
    if (!wallTimes_ms.empty()) {
      _stream << "  ------------------------------- WALL Time ------------------------------------\n"
              << "  min:           " << timed::utils::min(wallTimes_ms) << " ms\n"
              << "  max:           " << timed::utils::max(wallTimes_ms) << " ms\n"
              << "  mean:          " << timed::utils::mean(wallTimes_ms) << " ms\n"
              << "  median:        " << timed::utils::median(wallTimes_ms) << " ms\n"
              << "  % err          " << timed::utils::medianAbsolutePercentError(wallTimes_ms) << "\n";
    }
  }
  else {
    if (!cpuTimes_ms.empty()) {
      _stream << "  -------------------------------- CPU Time ------------------------------------\n"
              << "  cpu time:      " << cpuTimes_ms[0] << " ms\n";
    }
    if (!wallTimes_ms.empty()) {
      _stream << "  ------------------------------- WALL Time ------------------------------------\n"
              << "  wall time:     " << wallTimes_ms[0] << " ms\n";
    }
  }
  _stream << std::flush;
}

// ===== CSVReporter ===================================================================================================
// ----- public --------------------------------------------------------------------------------------------------------
// _____________________________________________________________________________________________________________________
void CSVReporter::ReportInit(const std::string &launcherName) {
  _stream << "name" << _separator << "description" << _separator << "iterations" << _separator << "cpu-min [ns]"
          << _separator << "cpu-max [ns]" << _separator << "cpu-mean [ns]" << _separator << "cpu-median [ns]"
          << _separator
          << "cpu-%err" << _separator << "wall-min [ns]" << _separator << "wall-max [ns]" << _separator
          << "wall-mean [ns]"
          << _separator << "wall-median [ns]" << _separator << "wall-%err\n";
}

// _____________________________________________________________________________________________________________________
void CSVReporter::ReportBenchmark(BenchmarkBase *benchmark) {
  if (benchmark->_results.empty()) {
    _stream << _separator << _separator << _separator << _separator << _separator << _separator << _separator
            << _separator << _separator << _separator << _separator << _separator << '\n';
    _stream << std::flush;
    return;
  }
  std::vector<uint64_t> cpuTimes_ns;
  std::vector<uint64_t> wallTimes_ns;

  for (auto &res: benchmark->_results) {
    if (res.wallTime.getNanoseconds() != 0) { wallTimes_ns.push_back(res.wallTime.getNanoseconds()); }
    if (res.cpuTime.getNanoseconds() != 0) { cpuTimes_ns.push_back(res.cpuTime.getNanoseconds()); }
  }
  _stream << benchmark->_name << _separator << benchmark->_description << _separator << benchmark->_iterations
          << _separator << timed::utils::min(cpuTimes_ns)
          << _separator << timed::utils::max(cpuTimes_ns) << _separator << timed::utils::mean(cpuTimes_ns) << _separator
          << timed::utils::median(cpuTimes_ns) << _separator
          << timed::utils::medianAbsolutePercentError(cpuTimes_ns) << _separator << timed::utils::min(wallTimes_ns)
          << _separator << timed::utils::max(wallTimes_ns) << _separator << timed::utils::mean(wallTimes_ns)
          << _separator << timed::utils::median(wallTimes_ns) << _separator
          << timed::utils::medianAbsolutePercentError(wallTimes_ns) << "\n";
  _stream << std::flush;
}

// ===== CompareReporter ===============================================================================================
// ----- public --------------------------------------------------------------------------------------------------------
// _____________________________________________________________________________________________________________________
void CompareReporter::Report(std::vector<std::shared_ptr<BenchmarkBase>> &benchmarks) {
  // TODO: implement
}

}  // namespace benchmarked