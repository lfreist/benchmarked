// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <map>

#include "benchmarked/benchmark_base.h"
#include "benchmarked/reporter.h"

#ifndef BENCHMARKED_LAUNCHER_H_
#define BENCHMARKED_LAUNCHER_H_

namespace benchmarked {

/**
 * Launcher: holds a collection of benchmarks and launches them on Launcher::Execute() call.
 *  Launcher can not be initialized manually but only via the static Launcher::GetInstance() method which returns
 *  a Launcher-Instance.
 */
class Launcher {
 public:
  Launcher() = default;
  explicit Launcher(const std::string& name);
  Launcher(const Launcher&) = delete;
  Launcher(Launcher&&) = delete;
  ~Launcher() = default;

  Launcher& operator=(const Launcher&) = delete;
  Launcher& operator=(Launcher&&) = delete;

  virtual void Launch(const std::string& nameFilter = "", const std::string& typeFilter = "");
  void RegisterBenchmark(const std::shared_ptr<BenchmarkBase>& benchmark);
  void RegisterBenchmarkBuilder(const std::function<std::shared_ptr<BenchmarkBase>()>& builder);
  void ClearAllBenchmarks();
  void ClearAllBenchmarksBuilders();

  void Report(std::unique_ptr<Reporter> reporter);
  //void Compare(std::unique_ptr<CompareReporter> reporter);

 protected:
  std::string _name;
  std::vector<std::shared_ptr<BenchmarkBase>> _benchmarks;
  std::vector<std::function<std::shared_ptr<BenchmarkBase>()>> _builders;
};


class LauncherConsole : public Launcher {
 public:
  LauncherConsole(const LauncherConsole&) = delete;
  LauncherConsole(LauncherConsole&&) = delete;
  virtual ~LauncherConsole() = default;

  LauncherConsole& operator=(const LauncherConsole&) = delete;
  LauncherConsole& operator=(LauncherConsole&&) = delete;

  static LauncherConsole& GetInstance();

  void Initialize(int argc, char** argv);
  void Execute();
  void Report();

 private:
  LauncherConsole() = default;

  bool _initialized = false;
  bool _list = false;
  std::string _nameFilter;
  std::string _typeFilter;
  const std::string _outputType = "console";
  const std::string _outputFile;
  std::ostream& _ostream = std::cout;
};

}  // namespace benchmarked

#endif //BENCHMARKED_LAUNCHER_H_
