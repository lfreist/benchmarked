// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#include <thread>
#include <chrono>

#include "benchmarked/benchmarked.h"

BENCHMARK("fibonacci_100", "example", "compute first 100 fibonacci numbers starting at third", 5) {
  int t1 = 0;
  int t2 = 1;
  int tmp;
  for (int i = 3; i <= 1000; ++i) {
    tmp = t1 + t2;
    t1 = t2;
    t2 = tmp;
  }
}

BENCHMARK("sleeping", "example", "sleep for 100 ms", 5) {
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

class ExampleFixture : public virtual benchmarked::Fixture {
 protected:
  std::string _haystack;
  std::string _needle = "keyword";

  void SetUp() override {
    _haystack = "This is some text containing a special word named keyword.";
  }
};

BENCHMARK_FIXTURE(ExampleFixture, "search for keyword", "example", "search using a fixture", 5) {
  _haystack.find(_needle);
}

BENCHMARK_MAIN()
