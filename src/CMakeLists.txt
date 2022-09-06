add_library(Benchmarked
        benchmark.cpp
        launcher.cpp
        reporter.cpp
        system.cpp
        )
target_link_libraries(Benchmarked PUBLIC boost_chrono timed::TimeUtils timed::Timer)

add_library(${PROJECT_NAME}::Benchmarked ALIAS Benchmarked)