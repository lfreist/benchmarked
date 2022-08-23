// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#include <regex>
#include <fstream>

#include "benchmarked/launcher.h"

namespace benchmarked {

// ===== Launcher ======================================================================================================
// ----- public --------------------------------------------------------------------------------------------------------
// _____________________________________________________________________________________________________________________
Launcher::Launcher(const std::string &name) {
  _name = name;
}

// _____________________________________________________________________________________________________________________
void Launcher::Launch(const std::string& nameFilter, const std::string& typeFilter) {
  for (const auto &builder: _builders) {
    RegisterBenchmark(builder());
  }

  std::regex nameMatcher(nameFilter);
  for (const auto &bm: _benchmarks) {
    if ((nameFilter.empty() || std::regex_match(bm->_name, nameMatcher)) && (typeFilter.empty() || typeFilter == bm->_type)) {
      bm->Launch();
    }
  }
}

// _____________________________________________________________________________________________________________________
void Launcher::RegisterBenchmark(const std::shared_ptr<BenchmarkBase> &benchmark) {
  _benchmarks.emplace_back(benchmark);
}

// _____________________________________________________________________________________________________________________
void Launcher::RegisterBenchmarkBuilder(const std::function<std::shared_ptr<BenchmarkBase>()> &builder) {
  if (builder) {
    _builders.emplace_back(builder);
  }
}

// _____________________________________________________________________________________________________________________
void Launcher::ClearAllBenchmarks() {
  _benchmarks.clear();
}

// _____________________________________________________________________________________________________________________
void Launcher::ClearAllBenchmarksBuilders() {
  _builders.clear();
}

// _____________________________________________________________________________________________________________________
void Launcher::Report(std::unique_ptr<Reporter> reporter) {
  reporter->ReportInit(_name);
  for (auto &bm: _benchmarks) {
    if (bm->_launched) {
      reporter->ReportBenchmark(bm.get());
    }
  }
}

// ===== ConsoleLauncher ===============================================================================================
// ----- public --------------------------------------------------------------------------------------------------------
// _____________________________________________________________________________________________________________________
LauncherConsole &LauncherConsole::GetInstance() {
  static LauncherConsole instance;
  return instance;
}

// _____________________________________________________________________________________________________________________
void LauncherConsole::Initialize(int argc, char **argv) {
  // TODO: implement
  _initialized = true;
}

// _____________________________________________________________________________________________________________________
void LauncherConsole::Execute() {
  if (_list) {
    std::regex nameMatcher(_nameFilter);
    for (const auto& bm: _benchmarks) {
      if ((_nameFilter.empty() || std::regex_match(bm->_name, nameMatcher)) && (_typeFilter.empty() || _typeFilter == bm->_type)) {
        std::cout << bm->_name << " - " << bm->_type << std::endl;
      }
    }
  }
  else {
    Launcher::Launch(_nameFilter, _typeFilter);
  }
}

// _____________________________________________________________________________________________________________________
void LauncherConsole::Report() {
  if (_outputType == "console") {
    if (!_outputFile.empty()) {
      std::ofstream ofs(_outputFile);
      Launcher::Report(std::make_unique<ConsoleReporter>(ConsoleReporter(ofs)));
    }
    else {
      Launcher::Report(std::make_unique<ConsoleReporter>(ConsoleReporter(std::cout)));
    }
  }
  else if (_outputType == "csv") {
    if (!_outputFile.empty()) {
      std::ofstream ofs(_outputFile);
      Launcher::Report(std::make_unique<CSVReporter>(CSVReporter(ofs)));
    }
    else {
      Launcher::Report(std::make_unique<CSVReporter>(CSVReporter(std::cout)));
    }
  }
}

}  // namespace benchmarked