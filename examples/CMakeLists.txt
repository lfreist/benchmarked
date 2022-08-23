add_executable(Examples benchmark_examples.cpp)
target_link_libraries(Examples PUBLIC Benchmarked hwinfo::HWinfo)