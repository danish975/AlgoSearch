#pragma once

#include "types.hpp"
#include <vector>
#include <functional>
#include <queue>

namespace algosearch {

/**
 * Min-Heap based Top-K retrieval.
 * 
 * Efficiently selects the top K highest-scored results from N candidates
 * using a min-heap of size K. This avoids sorting the entire candidate set.
 * 
 * Complexity:
 *   Processing N candidates: O(N log K)
 *   Final sorted output: O(K log K)
 *   
 *   This is optimal when K << N. For K close to N, a full sort may be
 *   comparable, but the heap approach uses O(K) extra space vs O(N).
 */
class TopKRetriever {
public:
    explicit TopKRetriever(int k = 10);

    // Process a single candidate
    void push(const ScoredResult& result);

    // Process a batch of candidates
    void push_batch(const std::vector<ScoredResult>& results);

    // Get the top-K results, sorted by score descending
    // This drains the heap.
    std::vector<ScoredResult> get_results();

    // Get current size of the heap
    size_t size() const;

    // Reset with a new K value
    void reset(int k);

    // Get the current minimum score in the heap (threshold for admission)
    double min_score() const;

    // Check if heap is full
    bool is_full() const;

private:
    int k_;

    // Min-heap: smallest score at top, so we can efficiently evict
    // the lowest-scoring element when a better candidate arrives
    std::priority_queue<
        ScoredResult,
        std::vector<ScoredResult>,
        std::greater<ScoredResult>
    > heap_;
};

} // namespace algosearch
