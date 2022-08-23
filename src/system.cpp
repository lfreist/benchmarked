// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#if defined(unix) || defined(__unix) || defined(__unix__) || defined(__APPLE__)
#include <thread>
#elif defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#endif

#include "benchmarked/system.h"

namespace benchmarked::system {

uint64_t currentThreadId() {
#if defined(unix) || defined(__unix) || defined(__unix__) || defined(__APPLE__)
  return static_cast<uint64_t>(pthread_self());
#elif defined(_WIN32) || defined(_WIN64)
  return GetCurrentThreadId();
#else
    #error Unsupported platform
#endif
}

}  // namespace benchmarked::system