#include "algosearch/search_engine.hpp"
#include "algosearch/types.hpp"
#include <iostream>
#include <chrono>
#include <random>
#include <vector>
#include <string>
#include <iomanip>
#include <numeric>
#include <algorithm>

using namespace algosearch;

// Generate synthetic problems for benchmarking
std::vector<Problem> generate_problems(int count) {
    std::vector<std::string> topics = {
        "Array", "String", "Hash Table", "Dynamic Programming", "Math",
        "Sorting", "Greedy", "DFS", "BFS", "Binary Search",
        "Tree", "Graph", "Heap", "Stack", "Queue",
        "Linked List", "Trie", "Backtracking", "Divide and Conquer",
        "Two Pointers", "Sliding Window", "Bit Manipulation",
        "Union Find", "Topological Sort", "Matrix"
    };

    std::vector<std::string> algorithms = {
        "Hash Map", "Binary Search", "DFS", "BFS", "Dijkstra",
        "Dynamic Programming", "Greedy", "Merge Sort", "Quick Sort",
        "Heap", "Trie", "KMP", "Union Find", "Topological Sort",
        "Kadane's Algorithm", "Two Pointers", "Sliding Window",
        "Backtracking", "Recursion", "Memoization"
    };

    std::vector<std::string> difficulties = {"Easy", "Medium", "Hard"};

    std::mt19937 rng(42);  // fixed seed for reproducibility

    std::vector<Problem> problems;
    problems.reserve(count);

    for (int i = 0; i < count; i++) {
        Problem p;
        p.id = "P" + std::to_string(10000 + i);
        p.title = "Problem " + std::to_string(i) + " " +
                  topics[rng() % topics.size()] + " " +
                  algorithms[rng() % algorithms.size()];
        p.slug = "problem-" + std::to_string(i);
        p.description = "This is a benchmark problem involving " +
                        topics[rng() % topics.size()] + " and " +
                        algorithms[rng() % algorithms.size()] +
                        ". Solve efficiently using appropriate data structures.";
        p.difficulty = difficulties[rng() % 3];

        int num_topics = 1 + rng() % 4;
        for (int t = 0; t < num_topics; t++) {
            p.topics.push_back(topics[rng() % topics.size()]);
        }

        int num_algos = 1 + rng() % 3;
        for (int a = 0; a < num_algos; a++) {
            p.algorithms.push_back(algorithms[rng() % algorithms.size()]);
        }

        p.platform = "LeetCode";
        p.popularity = rng() % 1000;
        p.created_at = "2024-01-01T00:00:00.000Z";

        problems.push_back(p);
    }

    return problems;
}

struct BenchmarkResult {
    std::string name;
    double avg_ms;
    double median_ms;
    double p95_ms;
    double min_ms;
    double max_ms;
    int iterations;
};

BenchmarkResult run_benchmark(
    const std::string& name,
    std::function<void()> fn,
    int iterations = 1000
) {
    std::vector<double> times;
    times.reserve(iterations);

    for (int i = 0; i < iterations; i++) {
        auto start = std::chrono::high_resolution_clock::now();
        fn();
        auto end = std::chrono::high_resolution_clock::now();
        double ms = std::chrono::duration<double, std::milli>(end - start).count();
        times.push_back(ms);
    }

    std::sort(times.begin(), times.end());

    double avg = std::accumulate(times.begin(), times.end(), 0.0) / iterations;
    double median = times[iterations / 2];
    double p95 = times[static_cast<int>(iterations * 0.95)];

    return {name, avg, median, p95, times.front(), times.back(), iterations};
}

void print_result(const BenchmarkResult& r) {
    std::cout << std::left << std::setw(40) << r.name
              << " | Avg: " << std::setw(10) << std::fixed << std::setprecision(3) << r.avg_ms << "ms"
              << " | Med: " << std::setw(10) << r.median_ms << "ms"
              << " | P95: " << std::setw(10) << r.p95_ms << "ms"
              << " | Min: " << std::setw(10) << r.min_ms << "ms"
              << " | Max: " << std::setw(10) << r.max_ms << "ms"
              << " | N=" << r.iterations
              << std::endl;
}

int main() {
    std::cout << "=== AlgoSearch Benchmark Suite ===" << std::endl;
    std::cout << "Hardware: Host machine" << std::endl;
    std::cout << "Compiler: " <<
#ifdef _MSC_VER
        "MSVC " << _MSC_VER
#elif defined(__GNUC__)
        "GCC " << __GNUC__ << "." << __GNUC_MINOR__
#elif defined(__clang__)
        "Clang " << __clang_major__ << "." << __clang_minor__
#else
        "Unknown"
#endif
        << std::endl;
    std::cout << std::string(120, '-') << std::endl;

    std::vector<int> sizes = {1000, 10000, 50000};

    for (int size : sizes) {
        std::cout << "\n=== Dataset size: " << size << " problems ===" << std::endl;

        auto problems = generate_problems(size);

        // Benchmark: Index construction
        SearchEngine engine;
        auto index_result = run_benchmark(
            "Index construction (" + std::to_string(size) + ")",
            [&]() { engine.load_problems(problems); },
            10
        );
        print_result(index_result);

        // Load once for search benchmarks
        engine.load_problems(problems);

        // Benchmark: Search with inverted index
        auto search_result = run_benchmark(
            "Search (inverted index)",
            [&]() {
                SearchQuery q;
                q.query = "graph dijkstra";
                q.limit = 10;
                engine.search(q);
            }
        );
        print_result(search_result);

        // Benchmark: Autocomplete
        auto ac_result = run_benchmark(
            "Autocomplete",
            [&]() {
                engine.autocomplete("gra", 8);
            }
        );
        print_result(ac_result);

        // Benchmark: Related problems
        auto related_result = run_benchmark(
            "Related problems",
            [&]() {
                engine.get_related_problems("P10000", 10);
            },
            100
        );
        print_result(related_result);

        // Benchmark: Single-term search
        auto single_result = run_benchmark(
            "Single-term search",
            [&]() {
                SearchQuery q;
                q.query = "array";
                q.limit = 10;
                engine.search(q);
            }
        );
        print_result(single_result);

        // Benchmark: Filtered search
        auto filtered_result = run_benchmark(
            "Search with filters",
            [&]() {
                SearchQuery q;
                q.query = "dynamic programming";
                q.difficulty = "Medium";
                q.limit = 10;
                engine.search(q);
            }
        );
        print_result(filtered_result);

        auto stats = engine.get_stats();
        std::cout << "  Stats: " << stats.total_problems << " problems, "
                  << stats.index_terms << " index terms, "
                  << stats.trie_terms << " trie terms, "
                  << stats.graph_nodes << " graph nodes" << std::endl;
    }

    std::cout << "\n=== Benchmark Complete ===" << std::endl;
    return 0;
}
