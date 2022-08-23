cmake_minimum_required(VERSION 3.21)
project(benchmarked VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED 20)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")

set(MAIN_PROJECT OFF)
if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(MAIN_PROJECT ON)
endif()

add_subdirectory(third_party/timed)
include_directories(third_party/timed/include)

add_subdirectory(third_party/hwinfo)
include_directories(third_party/hwinfo/include)


include_directories(${PROJECT_SOURCE_DIR}/include/)

if (${MAIN_PROJECT})
    # ----- google test ------------------------------------------------------------------------------------------------
    add_subdirectory(third_party/googletest)
    include_directories(third_party/googletest/googletest/include)
endif()

add_subdirectory(src)

if (${MAIN_PROJECT})
    # ----- examples ---------------------------------------------------------------------------------------------------
    add_subdirectory(examples)
    # ----- tests ------------------------------------------------------------------------------------------------------
    add_subdirectory(test)

    include(CTest)
    #add_test()
endif()
