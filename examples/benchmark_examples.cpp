// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

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

class ExampleFixture : public virtual benchmarked::Fixture {
 protected:
  std::string _haystack = "This is some text containing a special word named keyword.";
  std::string _needle = "keyword";
};

BENCHMARK_FIXTURE(ExampleFixture, "search for keyword", "example", "search using a fixture", 5) {
  _haystack.find(_needle);
}

BENCHMARK_MAIN()
