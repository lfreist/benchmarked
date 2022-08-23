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
  virtual void Initialize() {}
  virtual void Cleanup() {}
};

}  // namespace benchmarked

#endif //BENCHMARKED_FIXTURE_H_
