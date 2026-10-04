cmake_minimum_required(VERSION 3.20)
project(algosearch-engine VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Compiler flags
if(MSVC)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -O2)
endif()

# ── Fetch dependencies ──────────────────────────────────────────────
include(FetchContent)

FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG        v3.11.3
    GIT_SHALLOW    TRUE
)

FetchContent_Declare(
    httplib
    GIT_REPOSITORY https://github.com/yhirose/cpp-httplib.git
    GIT_TAG        v0.15.3
    GIT_SHALLOW    TRUE
)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        v1.14.0
    GIT_SHALLOW    TRUE
)

FetchContent_MakeAvailable(json httplib googletest)

# ── Core library ────────────────────────────────────────────────────
set(ENGINE_SOURCES
    src/trie.cpp
    src/inverted_index.cpp
    src/min_heap.cpp
    src/graph.cpp
    src/string_matcher.cpp
    src/ranking_engine.cpp
    src/search_engine.cpp
)

add_library(algosearch_core STATIC ${ENGINE_SOURCES})
target_include_directories(algosearch_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(algosearch_core PUBLIC nlohmann_json::nlohmann_json)

# ── HTTP Server executable ──────────────────────────────────────────
add_executable(algosearch_server src/main.cpp src/http_server.cpp)
target_link_libraries(algosearch_server PRIVATE algosearch_core httplib::httplib)

# ── CLI tool ────────────────────────────────────────────────────────
add_executable(algosearch_cli src/cli.cpp)
target_link_libraries(algosearch_cli PRIVATE algosearch_core)

# ── Tests ───────────────────────────────────────────────────────────
enable_testing()

set(TEST_SOURCES
    tests/test_trie.cpp
    tests/test_inverted_index.cpp
    tests/test_min_heap.cpp
    tests/test_graph.cpp
    tests/test_string_matcher.cpp
    tests/test_ranking_engine.cpp
    tests/test_search_engine.cpp
)

add_executable(algosearch_tests ${TEST_SOURCES})
target_link_libraries(algosearch_tests PRIVATE algosearch_core GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(algosearch_tests)

# ── Benchmarks ──────────────────────────────────────────────────────
add_executable(algosearch_bench benchmarks/benchmark_search.cpp)
target_link_libraries(algosearch_bench PRIVATE algosearch_core)
