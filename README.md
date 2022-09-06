# benchmarked
C++ benchmarking framework

## Content

   * [Why benchmarked?](#why-benchmarked)
   * [Build benchmarked](#build-benchmarked)
   * [Usage and Examples](#usage-and-examples)
   * [Include benchmarked to cmake project](#include-benchmarked-in-your-cmake-project)

## Why benchmarked?
1. benchmarked builds with basic cmake
2. benchmarked is lightweight -> only 350 KiB (gcc, ubuntu)
3. less requirements (only git, cmake, c++-compiler)
3. easy usage

## Build `benchmarked`
> Requirements: git, cmake, gcc

1. Download repository:
    ```
    git clone https://github.com/lfreist/benchmarked
    ```
2. Setup repository:
    ```
    git submodule init && git submodule update
    ```
3. Build using cmake:
    ```
    mkdir build
    cd build
    cmake -DCMAKE_BUILD_TYPE=Release .. && make -j $(nproc)
    ```

## Usage and Examples

### Example 0: Benchmark a function call
```c++
#include "benchmarked/benchmarked.h"

// Benchmark `myFunction` 5 times
BENCHMARK("name", "type", "description", 5) {
  myFunction();
}

BENCHMARK_MAIN()
```

### Example 1: Benchmark with fixture
```c++
#include <vector>
#include "benchmarked/benchmarked.h"

template <typename T>
class MyFixture : public benchmarked::Fixture {
  protected:
   T data;
   
   MyFixture() = default;
   
   void Initialize() override {
     data = T();
   }
   
   void Cleanup() override {
     data.clear();
   }
};

// Benchmark `myFunction` 5 times
BENCHMARK_FIXTURE(MyFixture<std::vector<int>>, "name", "type", "description", 5) {
  for (int i = 0; i < 1000; ++i) {
    data.push_back(i);
  }
}

BENCHMARK_MAIN()
```


## Include `benchmarked` in your cmake project
1. Download `benchmarked` into your project (e.g. in `<project-root>/third_party/benchmarked`)
    ```
    mkdir third_party
    cd third_party
    git clone https://github.com/lfreist/benchmarked
    cd benchmarked
    git submodule init && git submodule update
    ```
2. Simply add the following to your `<project-root>/CMakeLists.txt` file:
    ```cmake
    # file: <project-root>/CMakeLists.txt
    
    add_subdirectory(third_party/benchmarked)
    include_directories(third_party/benchmarked/include)
    ```
3. Include `benchmarked` into your `.cpp/.h` files:
    ```c++
    // file: your_benchmarks.cpp
    
    #include "benchmarked/benchmarked.h"
    ```
4. Link it in cmake
    ```cmake
    add_executable(${PROJECT_NAME}_benchmarks your_benchmarks.cpp)
    target_link_libraries(${PROJECT_NAME}_benchmarks PUBLIC benchmarked::Benchmarked)
    ```