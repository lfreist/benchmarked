// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#pragma once



#ifndef BENCHMARKED_FIXTURE_H_
#define BENCHMARKED_FIXTURE_H_

namespace benchmarked {

class Fixture {
 public:
  Fixture() noexcept = default;
  Fixture(const Fixture&) noexcept = default;
  Fixture(Fixture&&) noexcept = default;
  virtual ~Fixture() noexcept = default;

  Fixture& operator=(const Fixture&) noexcept = default;
  Fixture& operator=(Fixture&&) noexcept = default;

 protected:
  // only called once for all benchmark iterations
  virtual void SetUp() {}
  // called before every benchmark iteration
  virtual void Initialize() {}
  // called after every benchmark iteration
  virtual void Reset() {}
  // only called once after all benchmark iterations
  virtual void CleanUp() {}
};

}  // namespace benchmarked

#endif //BENCHMARKED_FIXTURE_H_
